from ultralytics import YOLO
import cv2
import numpy as np
import time
import serial
import threading
import queue

class SerialManager:
    def __init__(self):
        self.ser = None
        self.write_queue = queue.Queue(maxsize=5)
        self.writer_thread = None
        self.running = False
        self.last_send_time = 0
        self.send_interval = 0 # 寫入佇列間隔
        
    def initialize_serial(self):
        """初始化串口連接"""
        ports_to_try = ['COM5']
        
        for port in ports_to_try:
            try:
                print(f"嘗試連接到 {port}...")
                self.ser = serial.Serial(
                    port = port,
                    baudrate = 115200,
                    timeout = 0,  # 變成非阻塞模式
                    write_timeout = 0,  # 變成非阻塞模式
                    bytesize = serial.EIGHTBITS,
                    parity = serial.PARITY_NONE,
                    stopbits = serial.STOPBITS_ONE
                )
                
                self.ser.reset_input_buffer()
                self.ser.reset_output_buffer()
                time.sleep(5)  # 等待Arduino重置
                
                print(f"成功連接到 {port}")
                self.start_writer_thread()
                return True
                
            except Exception as e:
                print(f"連接 {port} 失敗: {e}")
                continue
        
        print("連接失敗")
        return False
    
    def start_writer_thread(self):
        """啟動寫入線程"""
        self.running = True
        self.writer_thread = threading.Thread(target=self._writer_worker, daemon=True)
        self.writer_thread.start()
    
    def _writer_worker(self):
        """寫入線程工作函數"""
        while self.running:
            try:
                data = self.write_queue.get()
                
                if self.ser and self.ser.is_open:
                    try:
                        message = f"{data:.2f}\n"
                        self.ser.write(message.encode('utf-8'))
                        print(f"發送到Arduino: {message.strip()}")
                    except Exception as e:
                        print(f"寫入錯誤: {e}")
                
                self.write_queue.task_done()
                
            except queue.Empty:
                continue
            except Exception as e:
                print(f"寫入線程錯誤: {e}")

    def send_data(self, data):
        """將資料寫入佇列"""
        current_time = time.time()
        
        # 檢查發送間隔
        if current_time - self.last_send_time < self.send_interval:
            return
        
        try:
            # 如果隊列滿了，丟棄最舊的數據
            if self.write_queue.full():
                try:
                    self.write_queue.get_nowait()
                except queue.Empty:
                    pass
            
            self.write_queue.put_nowait(data)
            self.last_send_time = current_time
        except queue.Full:
            pass
    
    def close(self):
        """關閉串口和線程"""
        self.running = False
        if self.writer_thread and hasattr(self.writer_thread, "join"):
            self.writer_thread.join(timeout=1)
        if self.ser:
            self.ser.close()
            print("連接已斷開")
            
def get_info(frame, model):
    '''辨識動物圖片，並回傳辨識結果與畫面'''
    # 類別設定
    class_names = ['chicken', 'cow', 'feed', 'pig']
    target_classes = {'chicken', 'cow', 'feed', 'pig'}

    results = model(frame, verbose=False)[0]
    boxes = results.boxes

    detected_display = set()
    detected_coords = []

    if boxes is not None and boxes.data.numel() > 0:
        conf_mask = boxes.conf >= 0.8
        boxes.data = boxes.data[conf_mask]

        detected_classes = boxes.cls.int().tolist()
        detected_boxes = boxes.xywh.tolist()  # xywh: [x, y, w, h] x,y中心位置 w,h長寬

        for idx, cls_id in enumerate(detected_classes):
            if 0 <= cls_id < len(class_names):
                name = class_names[cls_id]
                if name in target_classes:
                    detected_display.add(name)
                    coords = detected_boxes[idx]
                    detected_coords.append((name, coords))

        annotated_frame = results.plot()

        y_offset = 60
        for obj in sorted(detected_display):
            cv2.putText(
                annotated_frame,
                f"Detected:{obj}",
                (10, y_offset),
                cv2.FONT_HERSHEY_SIMPLEX,
                0.7,
                (0, 255, 0),
                2
            )
            y_offset += 40
    else:
        annotated_frame = frame.copy()  # 沒有偵測結果時直接用原圖

    return annotated_frame, detected_coords

def determine_arm_actions(rec_info, serial_manager):
    '''根據辨識結果決定動作'''
    global first_animal_rec, sent_status, status, sent_count

    if not rec_info:
        return  # 沒有偵測到物件就跳出
    if first_animal_rec is None:
        first_animal_rec = rec_info[0]
    
    for name, coords in rec_info:
        x = coords[0]
        if x < width0 * rectangle_right and x > width0 * rectangle_left:
            if name == first_animal_rec[0]: # 如果是第一個辨識到的動物
                match name:
                    case "chicken": # 綠燈
                        status = 1111
                    case "cow": # 紅燈
                        status = 2222
                    case "pig": # 黃燈
                        status = 3333
            elif name == "feed": # 飼料
                status = 4444

            if sent_status != status: # 跟上一個送的資料不一樣才送
                serial_manager.send_data(status)
                sent_status = status
                sent_count += 1
                time.sleep(4)
                if name == first_animal_rec[0] and sent_count == 3: # 倒飼料
                    serial_manager.send_data(5555)
                    time.sleep(1)
    
    return first_animal_rec

def preprocess_image_green(frame):
    """預處理圖像，提取綠色區域"""
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    
    # 綠色範圍檢測
    lower_green = np.array([70, 80, 100])
    upper_green = np.array([182, 255, 255])
    green_mask = cv2.inRange(hsv, lower_green, upper_green)
    
    # 形態學操作
    kernel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (5, 5))
    mask_green = cv2.morphologyEx(green_mask, cv2.MORPH_CLOSE, kernel, iterations=1)
    mask_green = cv2.morphologyEx(mask_green, cv2.MORPH_OPEN, kernel, iterations=1)
    mask_green = cv2.GaussianBlur(mask_green, (3, 3), 0)
    
    return mask_green

def create_roi_mask():
    """創建ROI遮罩"""
    roi_mask = np.zeros((height1, width1), dtype = np.uint8)

    # 矩形ROI區域
    x1 = int(width1 * 0.5)
    y1 = int(height1 * 0.4)
    x2 = int(width1 * 0.7)
    y2 = int(height1 * 1.0)
    cv2.rectangle(roi_mask, (x1, y1), (x2, y2), 255, -1)

    return roi_mask

def calculate_error(mask_detection, scan_row_ratio):
    """計算誤差"""
    scan_row = int(min(height1 * scan_row_ratio, height1 - 1))
    row = mask_detection[scan_row]
    indices = np.where(row > 50)[0] # 取得綠色像素的x座標

    if len(indices) == 0:
        image_error = 0
    else:
        mid = np.mean(indices)
        image_error = (width1 // 2 - mid) + 61
        cv2.circle(frame_line, (int(mid), int(scan_row)), 5, (0, 255, 255), -1)

    return image_error

def initialize_cameras():
    print("開啟手臂攝影機")
    cap0 = cv2.VideoCapture(1) # 手臂攝影機
    cap0.set(cv2.CAP_PROP_BUFFERSIZE, 1)
    cap0.set(cv2.CAP_PROP_FPS, 30)
    print("開啟跟線攝影機")
    cap1 = cv2.VideoCapture(0) # 跟線攝影機
    cap1.set(cv2.CAP_PROP_BUFFERSIZE, 1)
    cap1.set(cv2.CAP_PROP_FPS, 30)

    if not cap0.isOpened() or not cap1.isOpened():
        print("無法開啟攝影機")
        exit()

    return cap0, cap1

def process_arm_camera(frame_arm, model):
    annotated_frame, rec_info = get_info(frame_arm, model)

    return annotated_frame, rec_info

def process_line_camera(frame_line):
    roi_mask = create_roi_mask()
    mask_green = preprocess_image_green(frame_line)
    detect_mask = cv2.bitwise_and(mask_green, roi_mask)
    image_error = calculate_error(detect_mask, 0.6)

    return detect_mask, image_error

def display_results(frame_line, detect_mask, image_error, annotated_frame, first_animal_rec, width0, height0):
    height1, width1 = frame_line.shape[:2]
    cv2.line(frame_line, (width1 // 2, 0), (width1 // 2, height1), (255, 255, 0), 1)
    cv2.putText(frame_line, f"Error: {image_error:.2f}", (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (255, 255, 0), 2)
    cv2.imshow("Red Line Detection", frame_line)
    cv2.imshow("Detection Mask", detect_mask)

    if first_animal_rec is not None:
        cv2.putText(annotated_frame, f"Target Animal:{first_animal_rec[0]}", (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (255, 255, 0), 2)
    else:
        cv2.putText(annotated_frame, "Target Animal:None", (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (255, 255, 0), 2)

    cv2.rectangle(annotated_frame, (int(width0 * rectangle_left), 0), (int(width0 * rectangle_right), int(height0)), (255, 255, 0), 1)
    cv2.line(annotated_frame, (int(width0 * 0.23), 0), (int(width0 * 0.23), height0), (0, 0, 255), 1)
    cv2.imshow("YOLOv8 Detection", annotated_frame)

def main():
    global first_animal_rec, sent_status, status, sent_count, height0, width0, height1, width1, frame_line, rectangle_left, rectangle_right
    first_animal_rec = None
    sent_status, status, sent_count = 0, 0, 0
    rectangle_left, rectangle_right = 0.50, 0.63 # 辨識動物的區域大小
    image_error = 0
    # 初始化序列埠
    serial_manager = SerialManager()
    if not serial_manager.initialize_serial():
        print("程式將僅使用攝影機功能")

    # 初始化攝影機
    cap0, cap1 = initialize_cameras()

    # 初始化 YOLO 模型
    model = YOLO(r'C:\Users\mark0\OneDrive\田間機器人\室內關\best.pt')

    while True: # 主循環
        ret_arm, frame_arm = cap0.read()
        ret_line, frame_line = cap1.read()
        frame_arm = cv2.resize(frame_arm, (640, 360))
        frame_line = cv2.resize(frame_line, (854, 480))
        height0, width0 = frame_arm.shape[:2]
        height1, width1 = frame_line.shape[:2]
        if not ret_arm:
            print("無法讀取手臂攝影機畫面")
            break
        if not ret_line:
            print("無法讀取跟線攝影機畫面")
            break

        # 辨識動物圖片
        annotated_frame, rec_info = process_arm_camera(frame_arm, model)

        # 處理動物辨識結果
        determine_arm_actions(rec_info, serial_manager)

        # 處理跟線畫面
        detect_mask, image_error = process_line_camera(frame_line)
        serial_manager.send_data(image_error)

        # 顯示畫面
        display_results(frame_line, detect_mask, image_error, annotated_frame, first_animal_rec, width0, height0)

        # 按 esc 鍵離開
        if cv2.waitKey(1) & 0xFF == 27:
            break

    cap0.release()
    cv2.destroyAllWindows()
    serial_manager.close()

if __name__ == "__main__":
    main()

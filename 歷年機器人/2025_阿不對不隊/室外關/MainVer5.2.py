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
        ports_to_try = ['COM4', 'COM5']
        
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
                time.sleep(2)  # 等待Arduino重置
                
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
        if self.writer_thread:
            self.writer_thread.join(timeout=1)
        if self.ser:
            self.ser.close()
            print("連接已斷開")

def preprocess_image_red(frame):
    """預處理圖像，提取紅色區域"""
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    
    # 紅色範圍檢測 [相機設定要調對比最低 飽和最高]
    lower_red1 = np.array([0, 128, 10])
    upper_red1 = np.array([5, 255, 255])

    # lower_red1 = np.array([165, 130, 190])
    # upper_red1 = np.array([180, 255, 255])
    mask1 = cv2.inRange(hsv, lower_red1, upper_red1)

    lower_red2 = np.array([145, 128, 10])
    upper_red2 = np.array([180, 255, 255])

    # lower_red2 = np.array([165, 130, 190])
    # upper_red2 = np.array([180, 255, 255])
    mask2 = cv2.inRange(hsv, lower_red2, upper_red2)

    # 合併紅色遮罩
    red_mask = cv2.bitwise_or(mask1, mask2)
    red_mask[red_mask <= 60] = 0
    
    # 形態學操作
    kernel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (9, 9))
    mask_red = cv2.morphologyEx(red_mask, cv2.MORPH_CLOSE, kernel, iterations=1)
    # mask_red = cv2.morphologyEx(mask_red, cv2.MORPH_OPEN, kernel, iterations=1)
    mask_red = cv2.GaussianBlur(mask_red, (3, 3), 0)
    
    return mask_red

def preprocess_image_gray(frame):
    """預處理圖像，提取灰色區域"""
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    
    # 灰色範圍檢測 [相機設定要調對比最低 飽和最高]
    lower_gray1 = np.array([110, 20, 130])
    upper_gray1 = np.array([120, 130, 255])
    mask1 = cv2.inRange(hsv, lower_gray1, upper_gray1)

    lower_gray2 = np.array([110, 20, 130])
    upper_gray2 = np.array([120, 130, 255])
    mask2 = cv2.inRange(hsv, lower_gray2, upper_gray2)

    # 合併灰色遮罩
    gray_mask = cv2.bitwise_or(mask1, mask2)
    gray_mask[gray_mask <= 100] = 0
    
    # 形態學操作
    kernel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (9, 9))
    mask_gray = cv2.morphologyEx(gray_mask, cv2.MORPH_CLOSE, kernel, iterations=1)
    mask_gray = cv2.morphologyEx(mask_gray, cv2.MORPH_OPEN, kernel, iterations=0)
    mask_gray = cv2.GaussianBlur(mask_gray, (5, 5), 0)
    
    return mask_gray

def create_roi_mask(shape):
    """創建ROI遮罩"""
    roi_mask = np.zeros((height, width), dtype = np.uint8)

    # 矩形ROI區域
    x1 = int(width * 0.15)
    y1 = int(height * 0.3)
    x2 = int(width * 0.85)
    y2 = int(height * 1.0)
    cv2.rectangle(roi_mask, (x1, y1), (x2, y2), 255, -1)

    return roi_mask

def calculate_edges(mask_detection, scan_row_ratio):
    """回傳左右紅線位置"""
    scan_row = min(int(height * scan_row_ratio), height - 1)
    row = mask_detection[scan_row]
    row_left = row[:(width // 2)]
    row_right = row[(width // 2):]

    indices_left = np.where(row_left)[0] # 取得紅色像素的x座標
    indices_right = np.where(row_right)[0]

    if len(indices_left) == 0 and len(indices_right) == 0:
        return None, None, scan_row
    
    elif len(indices_left) == 0 and len(indices_right) != 0:
        right = indices_right[0] + width // 2
        return None, right, scan_row
    
    elif len(indices_left) != 0 and len(indices_right) == 0:
        left = indices_left[-1]
        return left, None, scan_row
    
    else:
        left = indices_left[-1]
        right = indices_right[0] + width // 2
            
    return left, right, scan_row  # 左右都有

def determine_error(masked_detection, scan_row_ratio1, scan_row_ratio2):
    """決定輸出誤差"""
    # 取得左右像素位置
    left1, right1, scan_row1 = calculate_edges(masked_detection, scan_row_ratio1)
    left2, right2, scan_row2 = calculate_edges(masked_detection, scan_row_ratio2)
    # 繪製兩條偵測線
    cv2.line(frame, (0, scan_row1), (width, scan_row1), (255, 255, 0), 1)
    cv2.putText(frame, f"FIRST ROW", (0, int(height * scan_row_ratio1 - 5)), cv2.FONT_HERSHEY_SIMPLEX, 0.4, (255, 255, 255), 1)
    cv2.line(frame, (0, scan_row2), (width, scan_row2), (255, 255, 0), 1)
    cv2.putText(frame, f"SECOND ROW", (0, int(height * scan_row_ratio2 - 5)), cv2.FONT_HERSHEY_SIMPLEX, 0.4, (255, 255, 255), 1)
    # 顯示資訊
    cv2.putText(frame, f"{left2} | {right2}", (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0, 255, 0), 2)
    cv2.putText(frame, f"{left1} | {right1}", (10, 60), cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0, 255, 0), 2)
    # 兩條row都在
    if left1 is not None and right1 is not None and left2 is not None and right2 is not None:
        mid_point = (left1 + right1 + left2 + right2) // 4
        image_error = (mid_point - center) * -1.0
        text = f"Deviation: {image_error:.2f}"
        cv2.circle(frame, (mid_point, (scan_row1 + scan_row2) // 2), 5, (0, 255, 255), -1)
        return image_error, text
    
    if left1 is not None and right1 is not None: # 第一條row左右都存在
        if left2 is not None and right2 is None: # 小右轉
            image_error = -333.0
            text = "No second row's right | Turn right"
            return image_error, text

        elif right2 is not None and left2 is None: # 小左轉
            image_error = 333.0
            text = "No second row's left | Turn left"
            return image_error, text
        
        else: # 第二條row都不存在
            mid_point = (left1 + right1) // 2
            image_error = (mid_point - center) * -1.0
            text = f"Deviation: {image_error:.2f}"
            cv2.circle(frame, (mid_point, scan_row1), 5, (0, 255, 255), -1)
            return image_error, text
        
    elif left2 is not None and right2 is not None: # 第二條row左右都存在
        if left1 is not None and right1 is None: # 小右轉
            image_error = -333.0
            text = "No first row's right | Turn right"
            return image_error, text
        
        elif right1 is not None and left1 is None: # 小左轉
            image_error = 333.0
            text = "No first row's left | Turn left"
            return image_error, text
        
        else: # 第一條row都不存在
            mid_point = (left2 + right2) // 2
            image_error = (mid_point - center) * -1.0
            text = f"Deviation: {image_error:.2f}"
            cv2.circle(frame, (mid_point, scan_row2), 5, (0, 255, 255), -1)
            return image_error, text  

    if right1 is None and right2 is None and (left1 is not None or left2 is not None): # 右邊值不存在            
        image_error = -666.0 # 大右轉
        text = "No right line exists | Turn right"
        return image_error, text

    elif left1 is None and left2 is None and (right1 is not None or right2 is not None): # 左邊值不存在        
        image_error = 666.0 # 大左轉
        text = "No left line exists | Turn left"
        return image_error, text

    #完全沒有紅線
    image_error = 0.0
    text = "No red line detected"

    return image_error, text
            
def detect_pipe(masked_detection_gray, scan_row_ratio):
    "檢測水管"
    scan_row = min(int(height * scan_row_ratio), height - 1)
    row = masked_detection_gray[scan_row]
    indices = np.where(row)[0] # 取得灰色像素的x座標

    if len(indices) > 200: # 大於200個值true
        return True
    
    return False

def main():
    # 初始化串口管理器
    serial_manager = SerialManager()
    
    if not serial_manager.initialize_serial():
        print("程式將僅使用攝影機功能")
    
    # 打開攝影機
    cap = cv2.VideoCapture(0)
    
    if not cap.isOpened():
        print("無法打開攝影機，嘗試其他攝影機...")
        for i in range(0, 4):
            cap = cv2.VideoCapture(i)
            if cap.isOpened():
                print(f"成功打開攝影機 {i}")
                break
        else:
            print("無法打開任何攝影機")
            serial_manager.close()
            return
    
    # 設置攝影機參數
    cap.set(cv2.CAP_PROP_BUFFERSIZE, 1)
    cap.set(cv2.CAP_PROP_FPS, 30)
    
    roi_mask = None
    
    print("攝影機已打開，開始圖像識別...")
    print("按 ESC 鍵退出程式")
    
    # 主循環
    while True:
        global frame, height, width, center, min_line_width, slope
        min_line_width = 300 # 判定是否為單線的像素寬度
        slope = 5 # 判定斜率
        ret, resize_frame = cap.read()
        frame = cv2.resize(resize_frame, (854, 480))

        if not ret:
            print("無法讀取攝影機畫面")
            break
        
        height, width = frame.shape[:2]
        center = width // 2
        
        # 創建ROI遮罩
        if roi_mask is None:
            roi_mask = create_roi_mask((height, width))
        
        # 處理圖像
        mask_red = preprocess_image_red(frame)
        masked_detection_red = cv2.bitwise_and(mask_red, roi_mask)
        mask_gray = preprocess_image_gray(frame)
        masked_detection_gray = cv2.bitwise_and(mask_gray, roi_mask)
        
        # 計算誤差
        image_error, text = determine_error(masked_detection_red, 0.85, 0.6)  

        # 發送誤差到Arduino
        serial_manager.send_data(image_error)    

        # 水管檢測
        pipe_exist = detect_pipe(masked_detection_gray, 0.85)
        if pipe_exist:
            image_error = 999

        # 發送誤差到Arduino
        serial_manager.send_data(image_error)
        
        # 繪製中心線
        cv2.line(frame, (center, 0), (center, height), (255, 255, 0), 1)
        
        # 顯示資訊
        cv2.putText(frame, text, (10, 90), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0, 255, 0), 2)
        cv2.putText(frame, f"Error: {image_error:.2f}", (10, 120), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (255, 255, 0), 2)
        
        # 顯示畫面
        cv2.imshow("Detection Mask(GRAY)", masked_detection_gray)
        cv2.imshow("Red Line Detection", frame)
        cv2.imshow("Detection Mask(RED)", masked_detection_red)

        # 檢查退出條件
        if cv2.waitKey(1) & 0xFF == 27: # 按ESC
            break
    
    # 清理資源
    cap.release()
    cv2.destroyAllWindows()
    serial_manager.close()

if __name__ == "__main__":
    main()

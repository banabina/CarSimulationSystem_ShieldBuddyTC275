# CarSimulationSystem_ShieldBuddyTC275

### Overview
차량의 기능을 단순화하여 임베디드 시스템에 구현

### 주요기능
* 주행 제어
* 라이트 제어
* 기어 변속
* 후방 감지

### 제어 흐름도
<img src="https://user-images.githubusercontent.com/46805714/227703065-81e4e7f9-08bc-4854-804f-29de6de58156.png" width="521" height="233"/>

### 개발환경
* Aurix Development Studio

### 하드웨어
* Shield Buddy TC275 
* Easy Module Shield V1
* Motor Shield Rev3
* Ultra Sonic Sensor
* DC Motor
<img src="https://user-images.githubusercontent.com/46805714/227703229-1d71a4fa-0402-40e2-b84d-6c7a0e7191ab.png" width="521" height="233"/>


### 회로도
<img src="https://user-images.githubusercontent.com/46805714/227703176-6ab32e7d-fb87-4576-9865-845b6af2f822.png" width="538" height="292"/>

### 기능 상세
주행 제어
* 엑셀 입력 시 모터 가속
* 브레이크 입력 시 모터 멈춤 
* 기어 변속 상태에 따라 모터 회전 방향 결정 (D, R 모드)

라이트 제어
* 브레이크 조작 시 경고등 점화
* 후진 기어 일 때 엑셀 버튼 조작 시 경고등 점멸

기어 변속
* 가변 저항 값에 따라 기어 변속 상태 변경
* 변속 상태에 따라 기어 상태등 변경

후방 감지
* 초음파 센서로 거리 감지
* 물체의 거리에 따라 경고음 간격을 다르게 출력

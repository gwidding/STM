# STM32F429를 활용한 디지털 알람시계 제작

수행기간 : 2023.11.10 ~ 2023. 11.21  (약 10일)
업무 : 회로제작, STM32 펌웨어 개발  
사용기능 : Timer, GPIO, EXT_INT, ADC(with DMA), Flash 메모리, PWM  
사용 디바이스 : Buzzer, 조이스틱, 16x2 character LCD  
사용 툴 : STM32CubeIDE  
사용 보드 : NUCLEO-F429ZI


## 기능 흐름도
![기능흐름도](https://github.com/gwidding/STM/assets/135992700/92a8af1e-7003-4629-b3b9-2a064ae15ee7)

## 모드
**I. NORMAL STATE : 현재시간 출력**
<table>
  <tr>
    <td><img src="https://github.com/gwidding/STM/assets/135992700/ed02a1a2-fc53-4577-b74a-85a217ac77f5" width="350px" height="150px" alt="노멀모드"></td>
    <td>
      <ul>
        <li>조이스틱 스위치 클릭 &rarr; Time Setting 모드</li>
        <li>2초 이상 클릭 &rarr; Alarm Time Setting 모드</li>
        <li>더블 클릭 &rarr; Music Select 모드</li>
      </ul>
    </td>
  </tr>
</table>

**II. TIME SETTING : 현재시간 바꾸기**
<table>
  <tr>
    <td><img src="https://github.com/gwidding/STM/assets/135992700/6b729834-8dc0-4928-a9ca-53c96859ec39" width="350px" height="150px" alt="시간변경모드"></td>
    <td>
      <ul>
        <li>커서 구분 : A, H, M, S 텍스트로 표현</li>
        <li>조이스틱 좌우 : 커서 위치 이동 (ampm, 시, 분, 초)</li>
        <li>조이스틱 상하 : 시간 값 조정 (am/pm, 증가/감소)</li>
        <li>클릭 : 변겅사항 저장 & Normal 모드</li>
      </ul>
    </td>
  </tr>
</table>

**III. ALARM TIME SETTING : 알람 시간 바꾸기**
<table>
  <tr>
    <td><img src="https://github.com/gwidding/STM/assets/135992700/56153f45-adc4-4ac9-b01c-46716ba7c3a4" width="350px" height="150px" alt="알람시간변경모드"></td>
    <td>
      <ul>
        <li>커서 구분 : A, H, M, S 텍스트로 표현</li>
        <li>조이스틱 좌우 : 커서 위치 이동 (ampm, 시, 분, 초)</li>
        <li>조이스틱 상하 : 시간 값 조정 (am/pm, 증가/감소)</li>
        <li>클릭 : 변겅사항 저장 & Normal 모드</li>
      </ul>
    </td>
  </tr>
</table>

**IV. MUSIC SELECT : 알람음 선택**
<table>
  <tr>
    <td><img src="https://github.com/gwidding/STM/assets/135992700/a3eb7055-b754-4f4a-ae39-06cd62e86126" width="350px" height="150px" alt="알람음변경모드"></td>
    <td>
      <ul>
        <li>조이스틱 상하 : 노래 변경</li>
        <li>클릭 : 변겅사항 저장 & Normal 모드</li>
      </ul>
    </td>
  </tr>
</table>

## 회로 구성
![회로연결](https://github.com/gwidding/STM/assets/135992700/107a9e04-d9fc-4a27-9ee7-4c2ec581aef0)


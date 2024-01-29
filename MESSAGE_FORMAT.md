# Smart Option Message Format 규칙
## 0. 주의사항
__해당 정리 내용은 Nasdaq 거래소의 "Smar Option NBBO & Trades Product Specifications' 문서의 <u>Message Formats</u> 항목을 참고하여 작성하였습니다.  
오번역이 있을 수 있으므로 가급적 원문과 함께 보시기를 권고 드립니다.__

## 1. Data Appendage Lists
- Appendate Lists는 Market Data Message 마지막 부분에 표기됩니다.
- 아예 없거나 혹은 여러개의 Appendage Element가 있으며, 메세지의 끝 부분까지 연결됩니다.

### 1.1. 필드 설명
- Element Length: Appendage element의 길이 정보가 표기됩니다. 모든 Appendage Element는 표준 헤더가 존재하며, 헤더에 해당 정보가 표기됩니다.
-  [Format Code](#12-format-code): 해당 element 값이 어떤 포맷을 갖고 있는지를 표기합니다.
-  [Value Type Code](#13-value-type-code): 해당 element 값이 어떤 값을 표기하고 있는지를 나타냅니다. 메세지의 카테고리(Instrument Locate, Value Type Codes)에 따라 다른 값이 할당됩니다. 
-  Value portion: Format Code에 따라 다른 값을 가집니다.
-  상세 필드 Table은 원문 참조 바랍니다.

### 1.2 Format Code
|Format Code|Name|Description|
|:---:|:---:|:--|
|**01**|Short Form Decimal|소수점 2자리수 실수형 표기|
|**02**|Long Form Decimal|소수점 4자리수 실수형 표기|
|**03**|Extended Form Decimal|**Denominator Code**에 따라 소수점 표기 
|**07**|Byte Value|1Byte 값
|**08**|Short Value|2Bytes 값
|**09**|Int32 Value|4Bytes 값
|**10**|Int64 Value|8Bytes 값
|**15**|String|문자열|
|**16**|Date|날짜표기(EX. 240129)
|**21**|[Boolean](#./DATA_TYPES.md-)|불린값
|**22**|Char Value|문자값(1바이트)  

### 1.3 Value Type Code
#### 1.3.1. 필드 설명
- 각각의 Appendage Element들은 1바이트의 Value Type Code를 가집니다.
- 만약 Value Type 값이 255인 경우, 
- 'Byte Enumeration'과 'Bitmask' 포맷은 Byte Numeric Value Format 방식으로 저장됩니다.


#### 1.3.2. Appendage Value Types Table
|Value Type Code|Name|Format|Message Types|
|:---:|:---:|:--:|:--:|
|**001**|Root Symbol|String|Instrument Locate|
|**002**|Put/Call|Char|Instrument Locate|
|**003**|Expiration Date|Date|Instrument Locate|
|**004**|Strike Price|Decimal|Instrument Locate|
|**065**|High Price|Decimal|Value Update|
|**066**|Low Price|Decimal|Value Update|
|**067**|Last Price|Decimal|Value Update|
|**068**|Open Price|Decimal|Value Update|
|**070**|Total Volume|Numeric|Value Update|
|**071**|Net Change|Decimal|Value Update|
|**072**|Open Interest|Numeric|Value Update|
|**073**|Tick|Decimal|Value Update|
|**074**|Bid|Decimal|Value Update|
|**075**|Ask|Decimal|Value Update|
|**081**|Parent Symbol Locate|Numeric|Instrument Locate / Meta-Data|
|**100**|Underlying Price|Decimal|Value Update|
|**143**|Upstream Condition Details|String|Trades, Trade Cancel|
|**145**|Trade Report Detail|Byte Enum|Trades, Trade Cancel|
|**146**|Extended Report Flags|Numeric Value|Trades, Trade Cancel|  


# 2. System Event and Channel Messages
## 2.1 System Event(0x20)
### 2.1.1. 메세지 설명
- System Event 메세지는 Smart Option 서비스 시 발생하는 Signal Events 정보를 나타냅니다.

### 2.2.2. System Event Field Table
|Field Name|Offset|Length|Type|Notes|
|:---:|:---:|:--:|:--:|:--|
|Message Type|0|1|Numeric|0x20(=Sytem Event)|
|Timestamp|1|8|Numeric|자정부터 소요된 시간(나노초 단위)
|Event Code|9|4|Alpha|[Event Code 참고](#223-system-event-code)

### 2.2.3. System Event Code  
|Code|Explanation|
|:---:|:---|
|*NUL|메세지의 시작을 나타낸다. Market Data Session에서 처음 발송되는 패킷이다.|
|*EOM|메세지의 끝을 나타낸다. Market Data Session에서 마지막으로 발송되는 패킷이다.|


---

*Note: 이 문서는 Smartoption Message Format 에 대한 규칙을 설명합니다.*

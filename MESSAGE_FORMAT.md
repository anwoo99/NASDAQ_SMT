# Smart Option Message Format 규칙

## 목차
- [Smart Option Message Format 규칙](#smart-option-message-format-규칙)
  - [목차](#목차)
  - [0. 주의사항](#0-주의사항)
  - [1. Data Appendage Lists](#1-data-appendage-lists)
    - [1.1. 필드 설명](#11-필드-설명)
    - [1.2 Format Code](#12-format-code)
    - [1.3 Value Type Code](#13-value-type-code)
      - [1.3.1. 필드 설명](#131-필드-설명)
      - [1.3.2. Appendage Value Types Table](#132-appendage-value-types-table)
      - [1.3.3. Put/Call](#133-putcall)
    - [1.3.4. Trade Report Detail](#134-trade-report-detail)
      - [1.3.5. Extended Report Flags](#135-extended-report-flags)
- [2. System Event and Channel Messages](#2-system-event-and-channel-messages)
  - [2.1 System Event(0x20)](#21-system-event0x20)
    - [2.1.1. 메세지 설명](#211-메세지-설명)
    - [2.1.2. System Event Field Table](#212-system-event-field-table)
    - [2.1.3. System Event Code](#213-system-event-code)
  - [2.2 Channel Seconds(0x22)](#22-channel-seconds0x22)
    - [2.2.1. 메세지 설명](#221-메세지-설명)
    - [2.2.2. Channel Index Field Table](#222-channel-index-field-table)
    - [2.2.3. Protocol ID Table](#223-protocol-id-table)
- [3. Locate Messages](#3-locate-messages)
  - [3.1. 개요](#31-개요)
  - [3.2. Market Center Locate(0x30)](#32-market-center-locate0x30)
    - [3.2.1. 필드 설명](#321-필드-설명)
    - [3.2.2. Market Center Locate Field Table](#322-market-center-locate-field-table)
  - [3.3. Instrumental Symbol Locate(0x33)](#33-instrumental-symbol-locate0x33)
    - [3.3.1. 필드 설명](#331-필드-설명)
    - [3.3.2. Instrument Symbol Locate Field Table](#332-instrument-symbol-locate-field-table)
    - [3.3.3. Product Type Code Table](#333-product-type-code-table)
- [4. Market Data Message](#4-market-data-message)
  - [4.1. Standard Header](#41-standard-header)
    - [4.1.1. Standard Header Field Table](#411-standard-header-field-table)
  - [4.1.2. Message Flags Table](#412-message-flags-table)
  - [4.2. NBBO Message(0x60 ~ 0x65)](#42-nbbo-message0x60--0x65)
    - [4.2.1. Message Types](#421-message-types)
    - [4.2.2. NBBO Flags](#422-nbbo-flags)
  - [4.3. Trade Messages(0x70 ~ 0x73)](#43-trade-messages0x70--0x73)
    - [4.3.1. Message Types](#431-message-types)
    - [4.3.2. Price Flags](#432-price-flags)
    - [4.3.3. Eligibility Flags](#433-eligibility-flags)
    - [4.3.4. Report Flags](#434-report-flags)
    - [4.3.5. Change Flags](#435-change-flags)
    - [4.3.6. Cancel Flags](#436-cancel-flags)
  - [4.4 Value Updates(0x80)](#44-value-updates0x80)
  - [4.4.1. Value Update Field Table](#441-value-update-field-table)
  - [4.4.2. Value Update Flags](#442-value-update-flags)
  - [4.5. Instrument Status(0x90)](#45-instrument-status0x90)
    - [4.5.1. Instrument Status Fields Table](#451-instrument-status-fields-table)
    - [4.5.2. Status Type](#452-status-type)
    - [4.5.3. Status Code](#453-status-code)
  - [4.6. Other Market Data Messages](#46-other-market-data-messages)
    - [4.6.1. Channel Event(0xB0)](#461-channel-event0xb0)
      - [4.6.1.1 Channel Event Field Table](#4611-channel-event-field-table)
      - [4.6.1.2. Channel Event Code](#4612-channel-event-code)
    - [4.6.2. Administrative Text(0xB2)](#462-administrative-text0xb2)
- [5. Miscellaneous and Reference Data Messages](#5-miscellaneous-and-reference-data-messages)
  - [5.1. Instrument Meta Data(0xC0)](#51-instrument-meta-data0xc0)
    - [5.1.1. Instrument Meta Data Field Table](#511-instrument-meta-data-field-table)
  - [5.2. Option Delivery Component(0xC3)](#52-option-delivery-component0xc3)
    - [5.2.1. Instrument Meta Data Field Table](#521-instrument-meta-data-field-table)

## 0. 주의사항
__해당 정리 내용은 Nasdaq 거래소의 "Smar Option NBBO & Trades Product Specifications' 문서의 <u>Message Formats</u> 항목을 참고하여 작성하였습니다.  
오번역이 있을 수 있으므로 가급적 원문과 함께 보시기를 권고 드립니다.__

## 1. Data Appendage Lists
- Appendate Lists는 [Market Data Message](./DATA_TYPE.md#1-7-market-data-메세지) 마지막 부분에 표기됩니다.
- 아예 없거나 혹은 여러개의 Appendage Element가 있으며, 메세지의 끝 부분까지 연결됩니다.

### 1.1. 필드 설명
- Element Length: Appendage element의 길이 정보가 표기됩니다. 모든 Appendage Element는 표준 헤더가 존재하며, 헤더에 해당 정보가 표기됩니다.
-  [Format Code](#12-format-code): 해당 element 값이 어떤 포맷을 갖고 있는지를 표기합니다.
-  [Value Type Code](#13-value-type-code): 해당 element 값이 어떤 값을 표기하고 있는지를 나타냅니다. 메세지의 카테고리(Instrument Locate, Value Type Codes)에 따라 다른 값이 할당됩니다. 
-  Value portion: [Format Code](#12-format-code)에 따라 다른 값을 가집니다.
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
|**21**|[Boolean](./DATA_TYPE.md#)|불린값
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
|**002**|[Put/Call](#133-putcall)|Char|Instrument Locate|
|**003**|Expiration Date|Date|Instrument Locate|
|**004**|Strike Price|[Decimal](./DATA_TYPE.md#1-4-decimal)|Instrument Locate|
|**065**|High Price|[Decimal](./DATA_TYPE.md#1-4-decimal)|Value Update|
|**066**|Low Price|[Decimal](./DATA_TYPE.md#1-4-decimal)|Value Update|
|**067**|Last Price|[Decimal](./DATA_TYPE.md#1-4-decimal)|Value Update|
|**068**|Open Price|[Decimal](./DATA_TYPE.md#1-4-decimal)|Value Update|
|**070**|Total Volume|[Numeric](./DATA_TYPE.md#1-2-numeric-필드)|Value Update|
|**071**|Net Change|[Decimal](./DATA_TYPE.md#1-4-decimal)|Value Update|
|**072**|Open Interest|[Numeric](./DATA_TYPE.md#1-2-numeric-필드)|Value Update|
|**073**|Tick|[Decimal](./DATA_TYPE.md#1-4-decimal)|Value Update|
|**074**|Bid|[Decimal](./DATA_TYPE.md#1-4-decimal)|Value Update|
|**075**|Ask|[Decimal](./DATA_TYPE.md#1-4-decimal)|Value Update|
|**081**|Parent Symbol Locate|[Numeric](./DATA_TYPE.md#1-2-numeric-필드)|Instrument Locate / Meta-Data|
|**100**|Underlying Price|[Decimal](./DATA_TYPE.md#1-4-decimal)|Value Update|
|**143**|Upstream Condition Details|String|Trades, Trade Cancel|
|**145**|[Trade Report Detail](#134-trade-report-detail)|Byte Enum|Trades, Trade Cancel|
|**146**|[Extended Report Flags](#135-extended-report-flags)|[Numeric](./DATA_TYPE.md#1-2-numeric-필드)|Trades, Trade Cancel|  

#### 1.3.3. Put/Call
|Code|Explanation|
|:---:|:---|
|'P'|Put|
|'C'|Call|

### 1.3.4. Trade Report Detail
- Trade와 관련한 추가 정보를 제공한다.
|Code|Explanation|
|:---:|:---|
|00|None or Not Provided|
|07|Automatic Execution|
|12|Adjustment|
|13|Stopped Stock(No Trade Through)|

#### 1.3.5. Extended Report Flags
- 해당 플래그는 bitmask 방식으로 표현된다.
- Trade/Cancel/Correction 이벤트와 관련된 자주 발생하지 않는 안내 사항을 전달할 때 사용된다.
- 어떠한 플래그도 세팅되지 않으면 해당 appendage element는 전송되지 않는다.

|Code|Explanation|
|:---:|:---|
|0x0001|Out of Sequence|
|0x0002|Last Reporting|
|0x0020|Stopped|


<br/>

# 2. System Event and Channel Messages
## 2.1 System Event(0x20)
### 2.1.1. 메세지 설명
- System Event 메세지는 Smart Option 서비스 시 발생하는 Signal Events 정보를 나타냅니다.

### 2.1.2. System Event Field Table
|Field Name|Offset|Length|Type|Notes|
|:---:|:---:|:--:|:--:|:--|
|Message Type|0|1|[Numeric](./DATA_TYPE.md#1-2-numeric-필드)|0x20(=Sytem Event)|
|Timestamp|1|8|[Numeric](./DATA_TYPE.md#1-2-numeric-필드)|자정부터 소요된 시간(나노초 단위)
|Event Code|9|4|[Alpha](./DATA_TYPE.md#1-1-alpha-필드)|[Event Code 참고](#213-system-event-code)

### 2.1.3. System Event Code  
|Code|Explanation|
|:---:|:---|
|*NUL|메세지의 시작을 나타낸다. Market Data Session에서 처음 발송되는 패킷이다.|
|*EOM|메세지의 끝을 나타낸다. Market Data Session에서 마지막으로 발송되는 패킷이다.|

<br/>

## 2.2 Channel Seconds(0x22)
### 2.2.1. 메세지 설명
- Channel Seconds 메세지는 Market Data Source Protocol과 Channel Index의 
  [Timestamp](./DATA_TYPE.md#1-5-timestamp) 시간초 부분을 업데이트하는데 사용된다.
- 해당 메세지는 오직 특정 Protocol ID 의 Channel Index Timestamp 정보만을 업데이트한다.
- 따라서 Protocol ID 필드와 Channel Index를 기준으로 매핑하여 값을 업데이트 해야한다.

### 2.2.2. Channel Index Field Table
|Field Name|Offset|Length|Type|Notes|
|:---:|:---:|:--:|:--:|:--|
|Message Type|0|1|[Numeric](./DATA_TYPE.md#1-2-numeric-필드)|0x22(=Channel Seconds)|
|Protocol ID|1|1|[Numeric](./DATA_TYPE.md#1-2-numeric-필드)|[Source Protocol ID](#223-protocol-id-table)|
|Channel Index|2|1|[Numeric](./DATA_TYPE.md#1-2-numeric-필드)|Source Channel Index|
|Seconds|3|4|[Numeric](./DATA_TYPE.md#1-2-numeric-필드)|자정부터 소요된 시간초|

### 2.2.3. Protocol ID Table
|Code|Explanation|
|:---:|:---:|
|1부터 50|사용예정(미정)|
|51|OPRA v3 or v5(Binary)|
|52|OPRA v6(Binary, Pillar)
|53부터 255|사용예정(미정)|

<br/>

# 3. Locate Messages
## 3.1. 개요
- Locate Message는 종목 정보와 관련한 정보(종목코드, MIC 등)를 전달합니다.
- Locate Code는 한 채널 내에서 유일값을 가진다.(채널간에는 Locate Code가 겹칠 수 있다)
- Locate Code는 날짜마다 변경될 수 있다.(즉 당일의 데이터만 매핑하는 것이 안전하다) 

## 3.2. Market Center Locate(0x30)
### 3.2.1. 필드 설명
  Market Center 내에서 유일한 locate 값을 가진다.

### 3.2.2. Market Center Locate Field Table
|Field Name|Offset|Length|Type|Notes|
|:---:|:---:|:--:|:--:|:--|
|Message Type|0|1|[Numeric](./DATA_TYPE.md#1-2-numeric-필드)|0x30(=Market Center Locate)|
|Locate Code|1|2|[Numeric](./DATA_TYPE.md#1-2-numeric-필드)|Market Center Locate Code|
|MIC|3|4|[Alpha](./DATA_TYPE.md#1-1-alpha-필드)|[ISO 10383 Market Identifier Code](./ISO10383_MIC.csv)|

## 3.3. Instrumental Symbol Locate(0x33)
### 3.3.1. 필드 설명
- 모든 Instrument 사이에서 유일한 Locate 값을 가진다.
- Product Type Code는 Instrument의 product type을 의미한다.
- Currency Code, Country Code, MIC 필드는 Instrument에 구분 값을 부여하고 싶을 때 할당된다(필수는 아님, 없을 경우 space 처리)

### 3.3.2. Instrument Symbol Locate Field Table
|Field Name|Offset|Length|Type|Notes|
|:---:|:---:|:--:|:--:|:--|
|Message Type|0|1|[Numeric](./DATA_TYPE.md#1-2-numeric-필드)|0x33(=Instrument Locate)|
|Locate Code|1|4|[Numeric](./DATA_TYPE.md#1-2-numeric-필드)|Instrument Locate Code|
|Country Code|5|2|[Alpha](./DATA_TYPE.md#1-1-alpha-필드)|[ISO 3166 Country Code](./ISO3166_CountryCode.csv) (없는 경우 공백처리)|
|Currency Code|7|3|[Alpha](./DATA_TYPE.md#1-1-alpha-필드)|[ISO 4217 Currency Code](./ISO4217_CurrencyCode.csv) (없는 경우 공백처리) |
|MIC|10|4|[Alpha](./DATA_TYPE.md#1-1-alpha-필드)|[ISO 10383 Market Identifier Code](./ISO10383_MIC.csv)|
|Product Type|14|1|[Numeric](./DATA_TYPE.md#1-2-numeric-필드)|[Produt Type Table 참고](#333-product-type-code-table)
|Symbol Length|15|1|[Numeric](./DATA_TYPE.md#1-2-numeric-필드)|Symbol Field 길이정보|
|Symbol|16|가변|[Alpha](./DATA_TYPE.md#1-1-alpha-필드)|품목명|
|Appendage|16 + SL|가변||[Appendage List 참고](#132-appendage-value-types-table)|

### 3.3.3. Product Type Code Table
|Code|Explanation|
|:---:|:---:|
|00|알수없음|
|01|Equity|
|02|Option|
|03|Index|
|10|World Currency|
|12|Fixed Income|
|14|Futures Contract|
|23|Option Root|

# 4. Market Data Message
- 모든 Market Data Message는 표준 헤더로 시작한다.

## 4.1. Standard Header
### 4.1.1. Standard Header Field Table
|Field Name|Offset|Length|Type|Notes|
|:---:|:---:|:--:|:--:|:--|
|Message Type|0|1|[Numeric](./DATA_TYPE.md#1-2-numeric-필드)|
|Protocol ID|1|1|[Numeric](./DATA_TYPE.md#1-2-numeric-필드)|[Protocol ID 참고](#223-protocol-id-table)
|Channel Index|2|1|[Numeric](./DATA_TYPE.md#1-2-numeric-필드)|
|Message Flags|2|1|Bitmask|[Message Flags Table 참고](#412-message-flags-table)
|Upstream Sequenece Number|4|4|[Numeric](./DATA_TYPE.md#1-2-numeric-필드)|
|Upstream Nanos|8|4|[Numeric](./DATA_TYPE.md#1-2-numeric-필드)|Timestamp의 나노초 부분([Channel Seconds 참고](#22-channel-seconds0x22))

## 4.1.2. Message Flags Table
|Bitmask Value|Explanation|
|:---:|:---|
|0x01|재전송으로 인한 out of sequence|
|0x02|Recovery 메커니즘의 결과로서 발생한 데이터를 포함하고 있음. </br> 즉, Recovery Snapshot을 의미한다. 
|0x04|Smart Option System 시간 정보에 관한 Timestamp 메세지를 의미한다. </br>즉, Upstream Data Feed Time이 아니다. </br> 해당 필드는 Upstream Feed가 Timestamp 정보를 제공하지 않을 때 사용된다.
 
## 4.2. NBBO Message(0x60 ~ 0x65)
- NBBO 메세지는 다양한 포맷[(Short, Long, Extended)](./DATA_TYPE.md#1-4-decimal)으로 제공된다.
- Two-sided(매수 & 매도), One-sided(매수 혹은 매도)방식으로 제공된다.
- 현재 NBBO 메세지는 Appendage List를 제공하지 않는다. 
- __상세 필드 테이블은 원문 참고 바랍니다.__

### 4.2.1. Message Types
|Message Type|Name|
|:---:|:---:|
|0x60|Two Sided Short NBBO(소수점 2자리)|
|0x61|Two Sided Long NBBO(소수점 4자리)|
|0x62|Two Sided Extended NBBO(Denominator 값 참고)|
|0x63|One Sided Short NBBO(소수점 2자리)|
|0x64|One Sided Long NBBO(소수점 4자리)|
|0x65|One Sided Extended NBBO(Denominator 값 참고)|

### 4.2.2. NBBO Flags
|Code|Explanation|
|:---:|:---|
|0x01|Quote Sizes are Provided in Shares, not Lots|


## 4.3. Trade Messages(0x70 ~ 0x73)
- __상세 필드 테이블은 원문 참고 바랍니다.__
- 하기 FLAGS 설명 참고 바랍니다.  

[Price Flags](#432-price-flags) / [Eligibility Flags](#433-eligibility-flags) / [Report Flags](#434-report-flags) / [Change Flags](#435-change-flags) / [Cancel Flags](#436-cancel-flags)

 
### 4.3.1. Message Types
|Message Type|Name|
|:---:|:---:|
|0x70|Short Trade|
|0x71|Long Trade|
|0x72|Extended Trade|
|0x73|Traded Cancel|

</br>

### 4.3.2. Price Flags
- 해당 플래그는 현재 사용되지 않습니다.


</br>

### 4.3.3. Eligibility Flags
- 해당 플래그는 bitmask 방식으로 표현된다. 
- Trade/Cancel/Correction 이벤트와 함께 제공되며, 통계 값을 제공한다.
- 항상 통계값을 업데이트하는 것은 아니다. 
  
|Code|Explanation|
|:---:|:---|
|0x01|미사용|
|0x02|미사용|
|0x04|미사용
|0x08|마지막 체결가|
|0x10|총 체결량|
|0x20|고가, 저가|
|0x40|시가|
|0X80|종가|

</br>

### 4.3.4. Report Flags
- Trade Report Flags는 bitmask 방식으로 표현된다.
- Trade에서 발생하는 Event를 나타낸다.
- 예시로, US Equities Feed 에서 발생하는 Special Event는 Market Center의 공식 시가 또는 종가를 나타내지만,  
  실제 거래를 나타내지는 않음(즉, 거래량이 올라가지 않음)에도 불구하고 "Offical Price Report" bit와 함께  
  "Opening" 또는 "Closing" 비트와 함께 세팅된다. 이러한 값은 실제 거래의 Volume과는 상관이 없다.
- 반대로 "Opening" 과 "Closing" 비트는 실제 거래의 시가 또는 종가를 나타내며, 이때는 실제 거래의  
  Volume과도 상관이 있다. 

|Code|Explanation|
|:---:|:---|
|0x0001|Opening|
|0x0002|Closing|
|0x0004|Re-Opening|
|0x0008|Cross Trade|
|0x0010|Extended Hours|
|0x0020|Intermarket Sweep(ISO)
|0x0040|Trade Throgh Exempt(US Equities)
|0x0080|Odd Lot(Equities)
|0x0100|Official Price Report(Used in conjunction with Opening and Closing flags)
|0x0200|Floor(Futures)|
|0x0400|Summary(Used to flag auction summary)
|0x0800|Printable|

</br>

### 4.3.5. Change Flags
- 해당 플래그는 bitmask 방식으로 표현된다.
- 특정 품목에 대한 통계 필드를 업데이트 시 발생하는 이벤트 관련 메세지를 포함한다.
- 해당 플래그는 오직 체결과 관련한 통계 필드가 업데이트 될 때만 사용된다.
- 해당 필드로는 오직 총 체결량이나 Last Sale Price 같이 자주 발생하지 않는 업데이트 사항에만 사용되며,  
  그 외 고가, 저가 같은 필드는 Value Update Message를 통해 전달된다.

|Code|Explanation|
|:---:|:---|
|0x01|미사용|
|0x02|Market Center Volume Updated|
|0x04|미사용|
|0x08|Market Center Last Sale Price Update|

</br>

### 4.3.6. Cancel Flags
- 해당 플래그는 bitmask 방식으로 표현된다.

|Code|Explanation|
|:---:|:---|
|0x01|Opening(시작 체결에 대한 Cancel)|
|0x02|Error(취소 체결이 에러 데이터임)|
|0x04|제공되지 않은 기존 trade 세부사항|

## 4.4 Value Updates(0x80)
- Value Update는 스마트 옵션에서 제공되는 체결 이외의 정보를 제공한다.
- 당일의 고가 저가, Index Tick Value와 같은 요약 정보들을 제공한다.


## 4.4.1. Value Update Field Table
|Field Name|Offset|Length|Type|Notes|
|:---:|:---:|:--:|:--:|:--|
|Standard Header|0|12||0x80|
|Instrument Locate|12|4|[Numeric](./DATA_TYPE.md#1-2-numeric-필드)||
|Market Center Locate|16|2|[Numeric](./DATA_TYPE.md#1-2-numeric-필드)|0 If Not Applicable
|Value Update Flags|18|4|Bitmask|[Value Update Flags 참고](#)
|Appendage List|22|variable||[Appendage List 참고](#132-appendage-value-types-table)

## 4.4.2. Value Update Flags
|Code|Explanation|
|:---:|:---|
|0x0001|Summary|
|0x0002|미사용|
|0x0004|미사용|
|0x0008|End Of Day|
|0x0010|미사용|

</br>

## 4.5. Instrument Status(0x90)
- 특정 품목에 대한 상태 변화 사항을 제공한다. 예시로는 상장폐지, 거래 종료 등이 있다.
- Status Type 필드는 어떤 종류의 상태가 업데이트되었는지를 알려준다.
- Status Code 필드는 해당 Status Type이 어떻게 변경되었는지를 알려준다.
- Reason Coded는 상태 변화 이유를 설명한다.
- Reason Detail은 상태 변화 이유에 대한 세부 사항에 대해 알려준다.\
  
### 4.5.1. Instrument Status Fields Table
|Field Name|Offset|Length|Type|Notes|
|:---:|:---:|:--:|:--:|:--|
|Standard Header|0|12||0x90|
|Instrument Locate|12|4|[Numeric](./DATA_TYPE.md#1-2-numeric-필드)|
|Market Center Locate|16|2|[Numeric](./DATA_TYPE.md#1-2-numeric-필드)
|Status Type|18|1|[Numeric](./DATA_TYPE.md#1-2-numeric-필드)|[Status Type 참고](#452-status-type)
|Status Code|19|1|[Numeric](./DATA_TYPE.md#1-2-numeric-필드)|[Status Code 참고](#453-status-code)
|Reason Code|20|1|[Numeric](./DATA_TYPE.md#1-2-numeric-필드)|
|Status Flags|21|1|Bitmask|
|Reason Detail Length|22|1|[Numeric](./DATA_TYPE.md#1-2-numeric-필드)|Reason Detail 길이
|Reason Detail|23|variable|[Alpha](./DATA_TYPE.md#1-1-alpha-필드)|
|Appednage List|23+RL|variable|[Appendage List 참고](#132-appendage-value-types-table)

### 4.5.2. Status Type
|Code|Explanation|
|:---:|:---|
|1|Trading State|

### 4.5.3. Status Code
|Code|Explanation|
|:---:|:---|
|1|Trading|
|2|미사용|
|3|거래 종료|


## 4.6. Other Market Data Messages
### 4.6.1. Channel Event(0xB0)
- Channel Event 메세지는 특정 Upstream 채널에서 발행된 Market Event 들을 전달한다.
- Appendage List에서는 해당 메세지에 대해 추가 정보를 전달한다.

#### 4.6.1.1 Channel Event Field Table
|Field Name|Offset|Length|Type|Notes|
|:---:|:---:|:--:|:--:|:--|
|Standard Header|0|12||0xB0|
|Channel Event Code|12|4|[Alpha](./DATA_TYPE.md#1-1-alpha-필드)|[Channel Event Code 참고](#4612-channel-event-code)
|Market Center Locate|16|2|[Numeric](./DATA_TYPE.md#1-2-numeric-필드)|
|Appednage List|18|variable||[Appendage List 참고](#132-appendage-value-types-table)

#### 4.6.1.2. Channel Event Code
|Code|Explanation|
|:---:|:---|
|*SOD|Start of Day|
|*EOD|End of Day|
|*EOT|End of Transaction(OPRA v3, v5 only)|
|*SOI|Start Open Interest|
|*EOI|End Open Interest|
|*SOS|Start of Summaries|
|*EOS|End of Summaries|
|*ECD|Early Close(OPRA v3, v5 only)|
|*GDM|Good Moring(OPRA v3, v5 only)|
|*GDN|Good Night(OPRA v3, v5 only)|

### 4.6.2. Administrative Text(0xB2)
|Field Name|Offset|Length|Type|Notes|
|:---:|:---:|:--:|:--:|:--|
|Standard Header|0|12||0xB2|
|Text Length|12|2|[Numeric](./DATA_TYPE.md#1-2-numeric-필드)|Text Field 길이|
|Text|14|variable|[Alpha](./DATA_TYPE.md#1-1-alpha-필드)|Text Message

</br>

# 5. Miscellaneous and Reference Data Messages
## 5.1. Instrument Meta Data(0xC0)
- 종목과 연관된 참고 데이터를 제공할 때 사용된다.

### 5.1.1. Instrument Meta Data Field Table
|Field Name|Offset|Length|Type|Notes|
|:---:|:---:|:--:|:--:|:--|
|Message Type|0|1|[Numeric](./DATA_TYPE.md#1-2-numeric-필드)|0xC0
|Locate Code|1|4|[Numeric](./DATA_TYPE.md#1-2-numeric-필드)|Symbol Locate for Option Root Code|
|Appendage List|5|variable||[Appendage List 참고](#132-appendage-value-types-table)

## 5.2. Option Delivery Component(0xC3)
- 비표준 옵션의 Delivery Component를 설명할 때 사용된다.
- Root Code에만 적용되는 데이터이다

### 5.2.1. Instrument Meta Data Field Table
|Field Name|Offset|Length|Type|Notes|
|:---:|:---:|:--:|:--:|:--|
|Message Type|0|1|[Numeric](./DATA_TYPE.md#1-2-numeric-필드)|0xC3
|Root Locate Code|1|4|[Numeric](./DATA_TYPE.md#1-2-numeric-필드)|Symbol Locate for Option Root Code|
|Component Index|5|4|[Numeric](./DATA_TYPE.md#1-2-numeric-필드)|해당 Root Code에 대해 전체 Component 중 </br> 몇 번째 Index 인지 표기
|Component Total|9|4|[Numeric](./DATA_TYPE.md#1-2-numeric-필드)|해당 Root Code에 대한 Delivery Component의 </br> 전체 수량. 0은 component가 없음을 의미한다.
|Deliverable Units|13|4|[Numeric](./DATA_TYPE.md#1-2-numeric-필드)|
|Settlement Method|17|1|[Numeric](./DATA_TYPE.md#1-2-numeric-필드)|정산 방법 </br> 1 = Cash </br> 2 = Continuous Net Settlement </br> 3 = Broker to Broker|
|Fixed Amount Denominator|18|1|[Numeric](./DATA_TYPE.md#1-2-numeric-필드)|Fixed Amount의 소수점 자릿수|
|Currency Code|27|3|[Alpha](./DATA_TYPE.md#1-1-alpha-필드)|[ISO 4217 Currency Code](./ISO4217_CurrencyCode.csv) </br> (Cash 정산이 아닌 경우 blank)
|Strike Percent|30|2|[Decimal](./DATA_TYPE.md#1-4-decimal)|Percentage of Strike Price allocated </br> for Settlement. <br> 소수점 2자리
|Component Symbol Locate|32|4|[Numeric](./DATA_TYPE.md#1-2-numeric-필드)|Symbol Locate For Component Symbol(Cash 정산인 경우 0)

---

*Note: 이 문서는 Smartoption Message Format 에 대한 규칙을 설명합니다.*

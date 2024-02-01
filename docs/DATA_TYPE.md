# Smartoption 데이터 유형 규칙

## 목차
- [Smartoption 데이터 유형 규칙](#smartoption-데이터-유형-규칙)
  - [목차](#목차)
  - [0. 주의사항](#0-주의사항)
  - [1. Data Types 규칙](#1-data-types-규칙)
    - [1-1. Alpha 필드](#1-1-alpha-필드)
    - [1-2. Numeric 필드](#1-2-numeric-필드)
    - [1-3. Signed / Unsigned](#1-3-signed--unsigned)
    - [1-4. Decimal](#1-4-decimal)
    - [1-5. Timestamp](#1-5-timestamp)
    - [1-6. Boolean](#1-6-boolean)
    - [1-7. Market Data 메세지](#1-7-market-data-메세지)
    - [1-8. Option 상품 제공방식](#1-8-option-상품-제공방식)


## 0. 주의사항
__해당 정리 내용은 Nasdaq 거래소의 "Smar Option NBBO & Trades Product Specifications' 문서의 <u>Data Types</u> 항목을 참고하여 작성하였습니다.  
오번역이 있을 수 있으므로 가급적 원문과 함께 보시기를 권고 드립니다.__

## 1. Data Types 규칙
### 1-1. Alpha 필드  
- **모든 고정 너비의 Alpha 필드**는 왼쪽 정렬되며, 우측의 빈 부분은 스페이스로 패딩됩니다.
### 1-2. Numeric 필드
- 모든 Numeric 필드는 빅엔디안 이진 형식입니다.
- Long(8바이트), Integer(4바이트), Shorts(2바이트), Bytes(1바이트)를 지원합니다.

### 1-3. Signed / Unsigned
1) Unsigned 필수 처리:
   - Size(quantities, volume, uints, totals),
   - locate codes, indexes, ratios, sequences, denominators  
  
2) Signed 필수 처리:
   - Price, numerators  

### 1-4. Decimal
   **Decimal**은 대역폭을 고려하여 여러 방법으로 제공됩니다.
   1) Short Form 
      - Short(2바이트) : 소수점 2자리 후처리
   2) Long Form  
      - Integer(4바이트) : 소수점 4자리 후처리
      - Long (8바이트) : Strike Price에서 사용, 소수점 10자리 후처리
   3) Expanded Form 
      - 1바이트의 denominator 값을 가짐
      - 계산 방법은 아래와 같다:
      $${Deciaml Value} = numerator \div (10^{denominator})$$
      
### 1-5. Timestamp
   1) Timestamp는 [numeric](#1-2-numeric-필드) 형식으로 표현되며, Eastern 시간 기준 당일 00:00부터 경과된 나노초로 저장됩니다.
   2) [Long(8바이트)](#1-2-numeric-필드) 또는 두 개의 정수 필드로 분리되어 표현되는데, 한 쪽은 초 단위가 나머지 한 쪽은 초 단위의 값을 제외한 나머지 나노초 단위가 저장됩니다.
   3) Market Data의 Timestamp는 보통 후자의 방법(정수 분리)을 사용하며, 'Channel Seconds(0x22)'의 'Seconds' 필드 + Market Data Standard Header의 'Upstream Nanos'를 함께 사용해야 합니다.

### 1-6. Boolean
- **Boolean Fields**는 [Numeric Bytes](#1-2-numeric-필드) 형식입니다.

### 1-7. Market Data 메세지
- 대부분의 **Market data 메세지**는 여러 개의 appendage element를 가진 Appendate List를 포함하고 있습니다. 해당 값은 굳이 처리를 안해도 되지만, 가급적 처리하는 것이 좋습니다.  

### 1-8. Option 상품 제공방식
   1) Option 상품의 거래(체결, 호가 등) 메세지들은 Instrument Locate 메세지 내의 'PARENT_LOCATE'라는 appendage type을 통해 'Underlying Symbol'과 연결됩니다.
   2) 아침에 여러 개의 'Underlying Symbol'이 발송됩니다(코라이즈 Master Data 와 비슷한 개념).
   3) 'Underlying Symbol'가 표시된 후 'Option Root' 라는 Product Type의 Instrument Locate 메세지가 뒤따라 표시됩니다.
   4) 'Option Root'들은 PARENT_LOCATE 값과, 여러 개의 Underlying Symbol locate code가 담긴 appendage 셋을 가지고 있습니다.  
   5) Special Settlement Delivery는 'Option Root'의 Locate Code와 함께 발송됩니다.
   6) Option 거래 메세지는 Symbol Locate, Option의 Product Type, Option Root에 해당하는 PARENT_LOCATE 값이 함께 발송됩니다.
   7) 전체적인 관계도는 [Option ⊃ Option Root ⊃ Underlying]입니다.
   8) 만약 Option Root와 Underlying을 재연결해야 하는 문제가 발생된다면, Option Root에 관한 Instrument Meta Data Message를 추가로 발송합니다.
   9) Special Settlement Delivery 및 Underlying Information은 만기일이나 Strike Price에 상관 없이 오직 한 개의 Option Root만을 참조합니다.

---

*Note: 이 문서는 Smartoption 데이터 유형에 대한 규칙을 설명합니다.*

# ArenaBattle

## **05.27**

> **<h3>Today Dev Story</h3>**

- <img src="Image/ReSharper.gif" height="200" title="ReSharper">
- ReSharper를 사용하여 자동 완성기능을 구현

- ### 고정된 분수대의 구현

   <details><summary>코드 보기</summary>

  ```c++
  //Fountain.h
  UPROPERTY(VisibleAnywhere)  //VisibleAnywhere->속성편집 가능하게 함, UPROPERTY->엔진이 컨트롤
  UStaticMeshComponent* Body;

  UPROPERTY(VisibleAnywhere)
  UStaticMeshComponent* Water;
  //Fountain.cpp
  //컴포넌트를 생성하는 코드
  Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BODY"));
  Water = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WATER"));
  //Body가 루트컴포넌트가 되며 Water는 Body의 자식이 된다.
  RootComponent = Body;
  Water->SetupAttachment(Body);
  	//좌표값 설정
  Water->SetRelativeLoacation(FVector(0.0f, 0.0f, 135.0f));
  ```

   </details>

> **<h3>Realization</h3>**

- 유니티와 다르게 종류에 따라서 C++이 자동으로 생성된다.
- 포인터로 선언된 객체는 "UPROPERTY" 매크로를 사용하여 엔진이 스스로 컨트롤 하게 한다.
  - 언리얼 오브젝트 클래스만 가능하며 아래와 같은 규칙이 존재한다.
    1. 클래스 선언 맥크로
       - 클래스 선언의 윗줄에 UCLASS 매크로 선언
       - 클래스 내부에는 GENERATED_BODY 매크로 선언
    2. 클래스 이름 접두사
       - 접두사가 A인 경우는 액터 클래스, 그 외에는 U라고 이름을 선언
       - 접두사가 F인 경우는 일반 C++ 클래스이다.
    3. generated.h 파일
       - 클래스를 분석하기 위함. 필수
    4. 외부 모듈에의 공개 여부
       - 클래스옆에 '모듈명\_API'를 선언해주어야 접근 가능
- 컴파일에 너무 많은 시간이 소요된다.

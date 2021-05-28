# ArenaBattle

## **05.28**

> **<h3>Today Dev Story</h3>**

- ### Rider을 사용하여 자동 완성기능을 구현 (기존 제공은 느리다.)

- ### 고정된 분수대의 구현
   - <img src="Image/Fountain.gif" height="300" title="Fountain"> <img src="Image/Asset_Auto_Set.gif" height="300" title="Asset_Auto_Set">
   - 3개의 컴포넌트(스태딕 메시, 라이트, 파티클)을 할당하여 구현
   - 에셋의 경로를 컴포넌트로 바로 지정하여 생성가능하게 구현
      <details><summary>코드 보기</summary>

      ```c++
      //Fountain.h
      UPROPERTY(VisibleAnywhere)	//VisibleAnywhere->속성편집 가능하게 함, UPROPERTY->엔진이 컨트롤
      UStaticMeshComponent *Body;

      UPROPERTY(VisibleAnywhere)
      UStaticMeshComponent *Water;

      UPROPERTY(VisibleAnywhere)
      UPointLightComponent *Light;

      UPROPERTY(VisibleAnywhere)
      UParticleSystemComponent *Splash;

      UPROPERTY(EditAnywhere, Category=ID)
      int32 ID;
      //Fountain.cpp

      //컴포넌트를 생성하는 코드
         Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BODY"));
      Water = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WATER"));
      Light = CreateDefaultSubobject<UPointLightComponent>(TEXT("LIGHT"));
      Splash = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("SPLASH"));

      //Body가 루트컴포넌트가 되며 Water는 Body의 자식이 된다.
      RootComponent = Body;
      Water->SetupAttachment(Body);
      Light->SetupAttachment(Body);
      Splash->SetupAttachment(Body);

      //좌표값 설정
      Water->SetRelativeLocation(FVector(0.0f,0.0f,135.0f));
      Light->SetRelativeLocation(FVector(0.0f,0.0f,195.0f));
      Splash->SetRelativeLocation(FVector(0.0f,0.0f,195.0f));

      //오브젝트의 키값을 가져와서 성공했다면 Body에 대입
      static ConstructorHelpers::FObjectFinder<UStaticMesh>
      SM_BODY(TEXT("/Game/InfinityBladeGrassLands/Environments/Plains/Env_Plains_Ruins/StaticMesh/SM_Plains_Castle_Fountain_01.SM_Plains_Castle_Fountain_01"));

      if(SM_BODY.Succeeded()) Body->SetStaticMesh(SM_BODY.Object);

      static ConstructorHelpers::FObjectFinder<UStaticMesh>
      SM_WATER(TEXT("/Game/InfinityBladeGrassLands/Effects/FX_Meshes/Env/SM_Plains_Fountain_02.SM_Plains_Fountain_02"));

      if(SM_WATER.Succeeded()) Water->SetStaticMesh(SM_WATER.Object);

      static ConstructorHelpers::FObjectFinder<UParticleSystem>
      SM_SPLASH(TEXT("/Game/InfinityBladeGrassLands/Effects/FX_Ambient/Water/P_Water_Fountain_Splash_Base_01.P_Water_Fountain_Splash_Base_01"));

      if(SM_SPLASH.Succeeded()) Splash->SetTemplate(SM_SPLASH.Object);
      ```

      </details>

- ### 움직이는 액터 설계
   - <img src="Image/Fountain_Rotate.gif" height="300" title="  Fountain_Rotate">
   1. 매크로를 사용하여 구현
      - Tick에서 구현하고 DeltaTime을 사용한다.
      - 컴포넌트를 추가하지 않아도 된다.
         <details><summary>코드 보기</summary>

         ```c++
         //Fountain.h
         private:
            UPROPERTY(EditAnywhere, Category=stat, Meta=(AllowPrivateAccess= true)) //private 타입이지만 Meta를 추가하여 접근가능
            float RotateSpeed;
         //Fountain.cpp
         AFountain::AFountain()
         {
            ...
            RotateSpeed = 30.0f;
         }
         void AFountain::Tick(float DeltaTime)
         {
            Super::Tick(DeltaTime);
            AddActorLocalRotation(FRotator(0.0f,RotateSpeed * DeltaTime,0.0f));  // Y, Z , X축 순서
         }
         ```

         </details>
      

   2. 무브먼트 컴포넌트 사용
      - Tick(Update)에서 사용하지 않아도 되고, DeltaTime을 사용하지 않는다.
      - 아래와 같이 다양한 움직임을 구사한다.
         - FloatingPawnMovement : 중력에 영향을 받지 않는 액터
         - RotatingMovement : 액터를 회전
         - InterpMovement : 액터를 지정된 좌표로 이동
         - ProjectMovement : 액터에 중력의 영향을 받아 포물선을 그리는 발사체 움직임

      <details><summary>코드 보기</summary>

      ```c++
      //Fountain.h
      #include "GameFramework/RotatingMovementComponent.h"
      ...
      UPROPERTY(VisibleAnywhere)
	   URotatingMovementComponent *Movement;
      //Fountain.cpp
	   Movement = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("MOVEMENT"));
	   Movement->RotationRate = FRotator(0.0f,RotateSpeed,0.0f);
      ```

      </details>

- ### 로그의 출력 방법
   - <img src="Image/Set_Log.png" height="200" title="Set_Log">
   - 게임 모듈에 로그를 추가하고 다른 모듈에서 끌어다 쓴다.
   - 그렇기에 다른 모듈(cpp)에는 항상 게임 모듈을 넣어둔다.

      <details><summary>코드 보기</summary>

      ```c++
      //Arena.cpp
      #include "Arena.h"
      #include "Modules/ModuleManager.h"

      DEFINE_LOG_CATEGORY(Arena);   //추가된 부분
      IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, Arena, "Arena" );

      //Arena.h
      #pragma once

      #include "EngineMinimal.h"       //추가된 부분

      DECLARE_LOG_CATEGORY_EXTERN(Arena, Log, All);   //추가된 부분
      //메크로 선언
      #define ABLOG_CALLINFO (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))
      #define ABLOG_S(Verbosity) UE_LOG(Arena, Verbosity, TEXT("%s"), *ABLOG_CALLINFO)
      #define ABLOG(Verbosity,Format,...) UE_LOG(Arena, Verbosity, TEXT("%s %s"),*ABLOG_CALLINFO,*FString::Printf(Format, ##__VA_ARGS__))

      //Fountain.h
      #include "Arena.h" //기존 "EngineMinmal.h"가 아닌 "Arena.h"참조

      //Fountain.cpp
      void AFountain::BeginPlay()
      {
         Super::BeginPlay();

         //UE_LOG(Arena,Warning,TEXT("Actor Name : %s, ID : %d, Location X : %.3f"),*this->GetName(),ID,GetActorLocation().X); -> 이전 방식
         //최근 방식
         ABLOG_S(Warning);
         ABLOG(Warning, TEXT("Actor Name : %s, ID : %d, Location X : %.3f"),*this->GetName(),ID,GetActorLocation().X);
      }
      ```

      </details>


- ### GameMode & Pawn & PlayerController 설정
   - <img src="Image/GameMode_set.png" height="200" title="GameMode_set"> 
   - ABGameMode, ABPawn, ABPlayerController을 생성하고 ABGameMode를 월드세팅에 GameMode에 넣어 룰을 지정한다.
   - ABPawn을 ABGameMode의 Default Pawn으로 지정한다. ABPlayerController또한 마찬가지 이다. (이때 생성이 아닌 클래스의 정보를 저장하는 것)
      <details><summary>코드 보기</summary>

      ```c++
      //ABGameMode.h
      class ARENA_API AABGameMode : public AGameModeBase
      {
         GENERATED_BODY()
      public:
         AABGameMode();
      };
      //ABGameMode.cpp
      #include "ABGameMode.h"
      #include "ABPawn.h"	      //내가 참조할것
      #include "ABPlayerController.h"

      AABGameMode::AABGameMode()
      {
         DefaultPawnClass = AABPawn::StaticClass();	//ABPawn의 클래스 정보를 저장 (멀티 플레이를 고려, 만들어두는 것이 아님)
         PlayerControllerClass = AABPlayerController::StaticClass();
      }
      ```

      </details>

- ### 플레이어의 입장
   - <img src="Image/Player_Login.png" height="200" title="Player_Login"> 
   - 플레이어가 로그인(입장)하면 언리얼 엔진에서 PostLogin 이벤트 함수가 호출된다. 
      - 이 과정에서 플레이어가 조종한 폰을 생성하고 플레이어 컨트롤러가 해당 폰에 빙의하는 작업이 이루어진다.
   - 각 생성되는 시점은 PostInitializeComponents, 빙의를 진행하는 시점은 PalyerController의 Posses, Pawn의 PossesedBy로 알 수 있다.
   - 아래는 PostLogin 이벤트 함수에 로그를 찍어서 플레이어 설정이 이루어지는 과정을 나타냈다.

      <details><summary>코드 보기</summary>

      ```c++
      //ABGameMode.h
      class ARENA_API AABGameMode : public AGameModeBase
      {
         GENERATED_BODY()
      public:
         AABGameMode();
      };
      //ABGameMode.cpp
      #include "ABGameMode.h"
      #include "ABPawn.h"	      //내가 참조할것
      #include "ABPlayerController.h"

      AABGameMode::AABGameMode()
      {
         DefaultPawnClass = AABPawn::StaticClass();	//ABPawn의 클래스 정보를 저장 (멀티 플레이를 고려, 만들어두는 것이 아님)
         PlayerControllerClass = AABPlayerController::StaticClass();
      }
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
- 에셋을 자동호출하기 위해서 주소를 키값으로 사용하며 초기화의 필요가 없음으로 static으로 사용한다.
- 로그는 게임모듈명에서 선언하고 다른 모듈들이 이 헤더를 참조하게 만든다.
- 액터의 이벤트 함수 : BeginPlay, Tick, EndPlay, PostInitalizeComponents ->액터의 셋팅이 
- 게임플레이를 도와주는 프레임워크 시스템 사용 (멀티플레이, 장르...)
- <img src="Image/Mode_Set.png" height="200" title="Mode_Set">

   - 폰의 타입, 시작 레벨을 지정한다.  

- <img src="Image/Player_login.jpg" height="200" title=" Player_login">

   - GameMode, PlayerController, Pawn의 입장 순서를 나타낸다.

## **05.29**
> **<h3>Today Dev Story</h3>**
   - null

> **<h3>Realization</h3>**
   - null
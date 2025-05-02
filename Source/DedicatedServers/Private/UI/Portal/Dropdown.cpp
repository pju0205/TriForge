// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Portal/Dropdown.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"


void UDropdown::NativeConstruct()
{
	Super::NativeConstruct();
	// 버튼 클릭 시 드롭다운 확장/축소 토글
	Button_Expander->OnClicked.AddDynamic(this, &UDropdown::ToggleDropdown);
	// 마우스 호버 시 텍스트 색상 변경
	Button_Expander->OnHovered.AddDynamic(this, &UDropdown::Hover);
	// 마우스가 벗어났을 때 텍스트 색상 원래대로 복원
	Button_Expander->OnUnhovered.AddDynamic(this, &UDropdown::Unhover);
}
 
void UDropdown::SetStyleTransparent()
{
	// 버튼의 모든 시각 스타일을 완전 투명으로 설정
	FButtonStyle Style;
	FSlateBrush Brush;
	Brush.TintColor = FSlateColor(FLinearColor(0.f, 0.f, 0.f, 0.f ));
	Style.Disabled = Brush;
	Style.Hovered = Brush;
	Style.Pressed = Brush;
	Style.Normal = Brush;

	// 투명 스타일을 버튼에 적용
	Button_Expander->SetStyle(Style);
}
 
void UDropdown::NativePreConstruct()
{
	Super::NativePreConstruct();

	// 에디터나 런타임 실행 전 스타일 초기화
	SetStyleTransparent();	// 버튼 투명화
	Collapse();				// 기본적으로 드롭다운은 접힘 상태
	Unhover();				// 기본 텍스트 색상으로 설정
}
 
void UDropdown::ToggleDropdown()
{
	// 드롭다운이 열려있으면 닫고, 닫혀있으면 열기
	if (bIsExpanded)
	{
		Collapse();
	}
	else
	{
		Expand();
	}
}
 
void UDropdown::Expand()
{
	// WidgetSwitcher로 확장된 위젯 보여주기
	WidgetSwitcher->SetActiveWidget(ExpandedWidget);
	bIsExpanded = true;								// 확장 상태 설정
	Image_Triangle->SetBrush(Triangle_Up);			// 삼각형 이미지 변경 (위 방향)
}
 
void UDropdown::Collapse()
{
	// WidgetSwitcher로 축소된 위젯 보여주기
	WidgetSwitcher->SetActiveWidget(CollapsedWidget);
	bIsExpanded = false;							// 확장 상태 설정
	Image_Triangle->SetBrush(Triangle_Down);		// 삼각형 이미지 변경 (아래 방향)
}
 
void UDropdown::Hover()
{
	// 마우스가 버튼 위에 올라갔을 때 텍스트 색상 변경
	TextBlock_ButtonText->SetColorAndOpacity(HoveredTextColor);
}
 
void UDropdown::Unhover()
{
	// 마우스가 버튼에서 벗어났을 때 텍스트 색상 복원
	TextBlock_ButtonText->SetColorAndOpacity(UnhoveredTextColor);
}
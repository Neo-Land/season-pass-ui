#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Styling/SlateBrush.h"
#include "Styling/SlateTypes.h"

/**
 * The season pass visual language in one place: palette, rounded brushes, type and button styles.
 * View-layer only. Nothing here reads or changes game state.
 */
namespace SeasonPassStyle
{
	namespace Color
	{
		inline const FLinearColor Backdrop  = FLinearColor(FColor(0x0B, 0x10, 0x20));
		inline const FLinearColor Panel     = FLinearColor(FColor(0x12, 0x18, 0x29));
		inline const FLinearColor Raised    = FLinearColor(FColor(0x1C, 0x24, 0x43));
		inline const FLinearColor RaisedHi  = FLinearColor(FColor(0x26, 0x30, 0x5A));
		inline const FLinearColor RaisedLo  = FLinearColor(FColor(0x14, 0x1A, 0x33));
		inline const FLinearColor Stroke    = FLinearColor(FColor(0x34, 0x40, 0x7A));
		inline const FLinearColor Text      = FLinearColor(FColor(0xE8, 0xEC, 0xF7));
		inline const FLinearColor TextDim   = FLinearColor(FColor(0x85, 0x90, 0xB0));
		inline const FLinearColor TextMuted = FLinearColor(FColor(0xAE, 0xB8, 0xD6));
		inline const FLinearColor Gold      = FLinearColor(FColor(0xFF, 0xC8, 0x3D));
		inline const FLinearColor GoldHi    = FLinearColor(FColor(0xFF, 0xDA, 0x70));
		inline const FLinearColor GoldLo    = FLinearColor(FColor(0xE6, 0xA8, 0x00));
		inline const FLinearColor GoldPale  = FLinearColor(FColor(0xFF, 0xF1, 0xC2));
		inline const FLinearColor Ink       = FLinearColor(FColor(0x1B, 0x14, 0x05));
		inline const FLinearColor Success   = FLinearColor(FColor(0x4A, 0xDE, 0x9B));
		inline const FLinearColor Skill     = FLinearColor(FColor(0x1F, 0xB5, 0x8F));
		inline const FLinearColor SkillHi   = FLinearColor(FColor(0x6F, 0xF2, 0xC8));
	}

	/** Rounded box with an optional outline. */
	inline FSlateBrush RoundedBrush(const FLinearColor& Fill, float Radius, const FLinearColor& Outline = FLinearColor::Transparent, float OutlineWidth = 0.f)
	{
		FSlateBrush Brush;
		Brush.DrawAs = ESlateBrushDrawType::RoundedBox;
		Brush.TintColor = FSlateColor(Fill);
		Brush.OutlineSettings = FSlateBrushOutlineSettings(Radius, FSlateColor(Outline), OutlineWidth);
		return Brush;
	}

	/** Sets size, weight, color and tracking while keeping the widget's font family. */
	inline void StyleText(UTextBlock* Text, float Size, FName Typeface, const FLinearColor& Color, int32 LetterSpacing = 0)
	{
		if (!Text)
		{
			return;
		}
		FSlateFontInfo Font = Text->GetFont();
		Font.Size = Size;
		Font.TypefaceFontName = Typeface;
		Font.LetterSpacing = LetterSpacing;
		Text->SetFont(Font);
		Text->SetColorAndOpacity(FSlateColor(Color));
		Text->SetShadowOffset(FVector2D::ZeroVector);
	}

	/** Rounded button with distinct normal/hover/pressed fills. */
	inline FButtonStyle MakeButtonStyle(const FLinearColor& Normal, const FLinearColor& Hovered, const FLinearColor& Pressed,
		float Radius, const FLinearColor& Outline = FLinearColor::Transparent, const FLinearColor& HoverOutline = FLinearColor::Transparent,
		FMargin Padding = FMargin(14.f, 8.f))
	{
		const float Width = Outline.A > 0.f || HoverOutline.A > 0.f ? 1.5f : 0.f;
		FButtonStyle Style;
		Style.SetNormal(RoundedBrush(Normal, Radius, Outline, Width));
		Style.SetHovered(RoundedBrush(Hovered, Radius, HoverOutline.A > 0.f ? HoverOutline : Outline, Width));
		Style.SetPressed(RoundedBrush(Pressed, Radius, Outline, Width));
		Style.SetDisabled(RoundedBrush(Normal * FLinearColor(1.f, 1.f, 1.f, 0.4f), Radius));
		Style.SetNormalPadding(Padding);
		Style.SetPressedPadding(FMargin(Padding.Left, Padding.Top + 1.f, Padding.Right, Padding.Bottom - 1.f));
		return Style;
	}

	/** Styles a button and the text label inside it. */
	inline void StyleButton(UButton* Button, const FButtonStyle& Style, float TextSize, const FLinearColor& TextColor)
	{
		if (!Button)
		{
			return;
		}
		Button->SetStyle(Style);
		Button->SetBackgroundColor(FLinearColor::White);
		StyleText(Cast<UTextBlock>(Button->GetContent()), TextSize, TEXT("Bold"), TextColor, 40);
	}
}

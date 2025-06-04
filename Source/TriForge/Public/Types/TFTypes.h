#pragma once
 
#include "TFTypes.generated.h"

//
// 게임 라운드 개인 승패
//
UENUM(BlueprintType)
enum class ERoundResult : uint8
{
	Draw	UMETA(DisplayName = "Draw"),
	Win		UMETA(DisplayName = "Win"),
	Loss	UMETA(DisplayName = "Loss")
};

//
// 필요한 정보 저장용
//
UENUM(meta = (Bitflags))
enum class ETFPlayerType : uint16
{
	None = 0,
	Wins			= 1 << 0,	// 00000000 00000001
	Losess			= 1 << 1,	// 00000000 00000010
	Kills			= 1 << 2,	// 00000000 00000100
	Deaths			= 1 << 3,	// 00000000 00001000
	RoundScore		= 1 << 4,	// 00000000 00010000
	MatchScore		= 1 << 5,	// 00000000 00100000
	
	/*
	None = 0,
	Headshot        = 1 << 0,    // 00000000 00000001
	Sequential      = 1 << 1,    // 00000000 00000010
	Streak          = 1 << 2,    // 00000000 00000100
	Revenge         = 1 << 3,    // 00000000 00001000
	Dethrone        = 1 << 4,    // 00000000 00010000
	Showstopper     = 1 << 5,    // 00000000 00100000
	FirstBlood      = 1 << 6,    // 00000000 01000000
	GainedTheLead   = 1 << 7,    // 00000000 10000000
	TiedTheLeader   = 1 << 8,    // 00000001 00000000
	LostTheLead   = 1 << 9,      // 00000010 00000000
	Defeats = 1 << 10,           // 00000100 00000000
	ScoredElims = 1 << 11,       // 00001000 00000000
	Hits = 1 << 12,              // 00010000 00000000
	Misses = 1 << 13             // 00100000 00000000
	*/
};

ENUM_CLASS_FLAGS(ETFPlayerType)
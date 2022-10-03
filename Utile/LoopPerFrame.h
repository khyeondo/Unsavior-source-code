// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>
#include "CoreMinimal.h"

/**
 * 
 */
class TEAMPROJECT_API LoopPerFrame
{
private:
	uint32 FunctionCallNumPerFrame = 1;
	uint32 LoopLength;
	int LoopCount = 0;

	bool Finished = false;

public:
	LoopPerFrame();
	~LoopPerFrame();

	void SetLength(const uint32 length);
	void SetCallNumPerFrame(const uint32 num);
	uint32 GetLoopCount() const;
	bool IsFinished();
	void Reset();

	bool operator()(std::function<void(int i)> func); //루프 종료시 true반환
	bool MulitpleLoop(LoopPerFrame& lowerLoopPerFrame, std::function<bool(int i)> func);
};

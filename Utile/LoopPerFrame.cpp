// Fill out your copyright notice in the Description page of Project Settings.


#include "LoopPerFrame.h"

LoopPerFrame::LoopPerFrame()
{
	FunctionCallNumPerFrame = 1;
	LoopLength = 1;
	LoopCount = 0;
}

LoopPerFrame::~LoopPerFrame()
{
}

bool LoopPerFrame::operator()(std::function<void(int i)> func)
{
	for (size_t i = 0; i < FunctionCallNumPerFrame; i++)
	{
		if (this->LoopCount >= (int)LoopLength) {
			Finished = true;
			return true;		
		}

		func(this->LoopCount);
		this->LoopCount++;
	}	
	return false;
}

bool LoopPerFrame::MulitpleLoop(LoopPerFrame& lowerLoopPerFrame, std::function<bool(int i)> func)
{
	for (size_t i = 0; i < FunctionCallNumPerFrame; i++)
	{
		if (this->LoopCount >= (int)LoopLength) {
			Finished = true;
			return true;
		}

		if (func(lowerLoopPerFrame.GetLoopCount()))
		{
			this->LoopCount++;
			lowerLoopPerFrame.Reset();
		}
	}
	return false;
}

bool LoopPerFrame::IsFinished()
{
	return Finished;
}

void LoopPerFrame::Reset()
{
	LoopCount = 0;
	Finished = false;
}

void LoopPerFrame::SetLength(const uint32 length)
{
	LoopLength = length;
}

void LoopPerFrame::SetCallNumPerFrame(const uint32 num)
{
	FunctionCallNumPerFrame = num;
}

uint32 LoopPerFrame::GetLoopCount() const
{
	return LoopCount;
}



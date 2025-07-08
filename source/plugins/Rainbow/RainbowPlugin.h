#pragma once

#include <FFGLSDK.h>

using namespace ffglqs;

class RainbowPlugin : public Effect
{
public:
	RainbowPlugin();
	~RainbowPlugin();

	std::shared_ptr<Param> scrollSpeedParam;
	std::shared_ptr<Param> mirrorParam;

	FFResult Render(ProcessOpenGLStruct* pGL) override;

private:
	float phase;
};

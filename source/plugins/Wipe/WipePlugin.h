#pragma once

#define PLUGIN_ID "HV06"

#include <FFGLSDK.h>

using namespace ffglqs;

class WipePlugin : public Effect
{
public:
	WipePlugin();
	~WipePlugin();

	std::shared_ptr<ParamTrigger> trigger;
	std::shared_ptr<Param> speed;
	std::shared_ptr<Param> width;
	std::shared_ptr<Param> angle;
	std::shared_ptr<ParamBool> mirror;

	void Update() override;
	FFResult Render( ProcessOpenGLStruct* inputTextures ) override;

private:
	struct Wipe
	{
		float phase;
		float speed;
		float width;
		float angle;
		bool mirror;
	};
	std::vector<Wipe> wipes;
};

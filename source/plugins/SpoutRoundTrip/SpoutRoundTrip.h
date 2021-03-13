#pragma once

#include "../../../Spout2/SPOUTSDK/SpoutGL/SpoutReceiver.h"
#include "../../../Spout2/SPOUTSDK/SpoutGL/SpoutSender.h"

#include <FFGLSDK.h>

using namespace ffglqs;

class SpoutRoundTrip : 
	public Effect
 
{
public:
	SpoutRoundTrip();
	~SpoutRoundTrip();

	FFResult InitGL( const FFGLViewportStruct* viewPort ) override;
	FFResult Render( ProcessOpenGLStruct* inputTextures ) override;
	FFResult DeInitGL() override;

	virtual char* GetParameterDisplay( unsigned int index ) override;
	virtual FFResult SetTextParameter( unsigned int index, const char* text ) override;
	virtual char* GetTextParameter( unsigned int index ) override;

private:
	const char* senderName;
	const char* receiverName;

	SpoutReceiver spoutReceiver;
	SpoutSender spoutSender;
};


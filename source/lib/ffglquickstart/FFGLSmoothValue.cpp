#include "FFGLSmoothValue.h"

namespace ffglqs
{
void SmoothValue::SetSmoothness( float smooth )
{
	smoothness = smooth;
}
void SmoothValue::Update( float val )
{
	if( onInit )
	{
		value = val;
		onInit = false;
	}
	else
	{
		value *= smoothness;
		value += ( 1 - smoothness ) * val;
	}
}
float SmoothValue::GetValue()
{
	return value;
}

}//End namespace ffglqs
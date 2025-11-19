#include "ItemCustomDataValue.h"

bool FItemCustomDataValue::operator==(const FItemCustomDataValue& Value) const
{
	return IntValue == Value.IntValue &&
		FloatValue == Value.FloatValue &&
		StringValue == Value.StringValue &&
		bBoolValue == Value.bBoolValue;
}

float FItemCustomDataValue::AsFloat(float Default) const
{
	if (FloatValue != 0.f) return FloatValue;
	if (IntValue != 0) return static_cast<float>(IntValue);
	if (bBoolValue) return 1.f;
	return Default;
}

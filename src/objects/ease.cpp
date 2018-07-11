#include <cmath>
#include "cinder/Easing.h"



#define DEF_EASE_FUN(name,ease) double do_ease_##name(int now, double start,double end,int duration) {\
float t = static_cast<float>(now) / duration; \
return (end-start) * cinder::ease(t)+start; \
}

#define DEF_EASE_FUNS(name,baseease) \
DEF_EASE_FUN(name##In,easeIn##baseease)\
DEF_EASE_FUN(name##Out,easeOut##baseease)\
DEF_EASE_FUN(name##InOut,easeInOut##baseease)\
DEF_EASE_FUN(name##OutIn,easeOutIn##baseease)

// extern "C" {
double do_ease_line(int now, double start,double end,int duration)
{
	float t = static_cast<float>(now) / duration;
	return (end-start) * cinder::easeNone(t) + start;
}

DEF_EASE_FUNS(quadratic,Quad);
DEF_EASE_FUNS(cubic,Cubic);
DEF_EASE_FUNS(quartic,Quart);
DEF_EASE_FUNS(quintic,Quint);
DEF_EASE_FUNS(sinusoidal,Sine);
DEF_EASE_FUNS(exponential,Expo)
DEF_EASE_FUNS(circular,Circ);
DEF_EASE_FUNS(bounce,Bounce);
// DEF_EASE_FUNS(elastic,Elastic);
DEF_EASE_FUNS(back,Back);

double do_ease_elasticIn(int now, double start,double end,int duration)
{

	float t = static_cast<float>(now) / duration;
	if (t <= 0.00001f) return start;
    if (t >= 0.999f) return end;
	float a = end - start;
	float p = 0.3f;
	return (end-start) * cinder::easeInElastic(t,a,p)+ start;
}

double do_ease_elasticOut(int now, double start,double end,int duration)
{

	float t = static_cast<float>(now) / duration;
	if (t <= 0.00001f) return start;
    if (t >= 0.999f) return end;
	float a = end - start;
	float p = 0.3f;
	return (end-start) * cinder::easeOutElastic(t,a,p)+ start;
}


double do_ease_elasticInOut(int now, double start,double end,int duration)
{

	float t = static_cast<float>(now) / duration;
	if (t <= 0.00001f) return start;
    if (t >= 0.999f) return end;
	float a = end - start;
	float p = 0.3f * 1.5f;
	return (end-start) * cinder::easeInOutElastic(t,a,p)+ start;
}

double do_ease_elasticOutIn(int now, double start,double end,int duration)
{

	float t = static_cast<float>(now) / duration;
	if (t <= 0.00001f) return start;
    if (t >= 0.999f) return end;
	float a = end - start;
	float p = 0.3f * 1.5f;
	return (end-start) * cinder::easeOutInElastic(t,a,p)+ start;
}

// }

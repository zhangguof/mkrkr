#include "tjsCommHead.h"

#include <deque>
#include <algorithm>
#include <cmath>
#include <time.h>
#include "TweenImpl.hpp"
//#include "tweeny.h"
#include "cinder/Easing.h"

/* Runs the tween with the designated easing */
// template<typename T,typename EasingT>
// tweeny::tween<T>&  via_method(tweeny::tween<T>& tween, EasingT easing)
// {
// 	return tween.via(easing);
// }

// DECLARE_EASING_RESOLVE(quadratic);
// DECLARE_EASING_RESOLVE(cubic);
// DECLARE_EASING_RESOLVE(quartic);
// DECLARE_EASING_RESOLVE(quintic);
// DECLARE_EASING_RESOLVE(sinusoidal);
// DECLARE_EASING_RESOLVE(exponential);
// DECLARE_EASING_RESOLVE(circular);
// DECLARE_EASING_RESOLVE(bounce);
// DECLARE_EASING_RESOLVE(elastic);
// DECLARE_EASING_RESOLVE(back);


// extern "C" {
typedef double (*t_ease_fun)(int,double,double,int);

const int t_ease_line = 1;

extern double do_ease_line(int now,double start,double end,int duration);
#define DEF_DO_EASE(name,val) \
extern double do_ease_##name##In(int now,double start,double end,int duration);\
const int t_ease_##name##In = (val);\
extern double do_ease_##name##Out(int now,double start,double end,int duration);\
const int t_ease_##name##Out = (val+1);\
extern double do_ease_##name##InOut(int now,double start,double end,int duration);\
const int t_ease_##name##InOut = (val+2);\
extern double do_ease_##name##OutIn(int now,double start,double end,int duration); \
const int t_ease_##name##OutIn = (val+3);

DEF_DO_EASE(quadratic,t_ease_line+1);
DEF_DO_EASE(cubic,t_ease_line+5);
DEF_DO_EASE(quartic, t_ease_line + 9);
DEF_DO_EASE(quintic, t_ease_line + 13);
DEF_DO_EASE(sinusoidal, t_ease_line + 17); //bug sinusoidalOutIn
DEF_DO_EASE(circular, t_ease_line + 21);
DEF_DO_EASE(exponential, t_ease_line + 25);
DEF_DO_EASE(elastic, t_ease_line + 29); //bug elasticOutIn
DEF_DO_EASE(back, t_ease_line + 33);
DEF_DO_EASE(bounce, t_ease_line + 37);
// }


#define RET_TWEEN(name) return do_ease_##name;

// #define RET_TWEEN(name) return tween.via(tweeny::easing::name)
// #define DEF_EASE(name,ext,val) const int t_ease_##name_##ext = (val);
// #define DEF_BASE_NAME(name,base_val) \
// const int t_ease_##name##In = (base_val);\
// const int t_ease_##name##Out = (base_val+1);\
// const int t_ease_##name##InOut = (base_val+2);\
// const int t_ease_##name##OutIn = (base_val+3);

#define CASE_EASE_ALL(name) case t_ease_##name##In:\
RET_TWEEN(name##In);\
case t_ease_##name##Out:\
RET_TWEEN(name##Out);\
case t_ease_##name##InOut:\
RET_TWEEN(name##InOut);\
case t_ease_##name##OutIn:\
RET_TWEEN(name##OutIn);


#define CASE_EASE(name,ext) case t_ease_##name_##ext:\
RET_TWEEN(name##ext);

// const int t_ease_line = 1;
// DEF_BASE_NAME(quadratic,t_ease_line+1);
// DEF_BASE_NAME(cubic,t_ease_line+5);
// DEF_BASE_NAME(quartic, t_ease_line + 9);
// DEF_BASE_NAME(quintic, t_ease_line + 13);
// DEF_BASE_NAME(sinusoidal, t_ease_line + 17); //bug sinusoidalOutIn
// DEF_BASE_NAME(circular, t_ease_line + 21);
// DEF_BASE_NAME(exponential, t_ease_line + 25);
// DEF_BASE_NAME(elastic, t_ease_line + 29); //bug elasticOutIn
// DEF_BASE_NAME(back, t_ease_line + 33);
// DEF_BASE_NAME(bounce, t_ease_line + 37);


// DEF_EASE(quadratic,In,2)
// DEF_EASE(quadratic,Out,3)
// DEF_EASE(quadratic,InOut,4)
// DEF_EASE(quadratic,OutIn,5)

// #define t_ease_quadratic_in 	(2)
// #define t_ease_quadratic_out 	(3)
// #define t_ease_quadratic_inout 	(4)
// #define t_ease_quadratic_outin 	(5)

t_ease_fun get_ease_func(int t)
{
	if(t<=0 || t>41) t = 1;
	switch(t)
	{
		case t_ease_line:
			RET_TWEEN(line);
	CASE_EASE_ALL(quadratic);
	CASE_EASE_ALL(cubic);
	CASE_EASE_ALL(quartic);
	CASE_EASE_ALL(quintic);
	CASE_EASE_ALL(sinusoidal);
	CASE_EASE_ALL(exponential);
	CASE_EASE_ALL(circular);
	CASE_EASE_ALL(bounce);
	CASE_EASE_ALL(elastic);
	CASE_EASE_ALL(back);
	default:
		break;
	}
	return do_ease_line;
}

// template<typename T>
// tweeny::tween<T>&  via_by_type(tweeny::tween<T>& tween,int t)
// {
// 	if(t<=0 || t>41) t = 1;
// 	switch(t)
// 	{
// 	case t_ease_line:
// 		RET_TWEEN(linear);
// 	CASE_EASE_ALL(quadratic);
// 	CASE_EASE_ALL(cubic);
// 	CASE_EASE_ALL(quartic);
// 	CASE_EASE_ALL(quintic);
// 	CASE_EASE_ALL(sinusoidal);
// 	CASE_EASE_ALL(exponential);
// 	CASE_EASE_ALL(circular);
// 	CASE_EASE_ALL(bounce);
// 	CASE_EASE_ALL(elastic);
// 	CASE_EASE_ALL(back);
// 	// CASE_EASE(quadratic,In)
// 	// CASE_EASE(quadratic,Out)
// 	// CASE_EASE(quadratic,InOut)
// 	// CASE_EASE(quadratic,OutIn)
// 	// case t_ease_quadratic_in:
// 	// 	RET_TWEEN(quadraticIn); 
// 	// case t_ease_quadratic_out:
// 	// 	RET_TWEEN(quadraticOut);
// 	// case t_ease_quadratic_inout:
// 	// 	RET_TWEEN(quadraticInOut);
// 	default:
// 		break;
// 	}
// 	return tween.via(tweeny::easing::linear);
// }




//---------------------------------------------------------------------------
// tTJSNI_SoundBuffer
//---------------------------------------------------------------------------
tTJSNI_Tween::tTJSNI_Tween()
{

}
//---------------------------------------------------------------------------
tjs_error TJS_INTF_METHOD
	tTJSNI_Tween::Construct(tjs_int numparams, tTJSVariant **param,
		iTJSDispatch2 *tjs_obj)
{
	tjs_error hr = inherited::Construct(numparams, param, tjs_obj);
	if(TJS_FAILED(hr)) return hr;

	return TJS_S_OK;
}
//---------------------------------------------------------------------------
void TJS_INTF_METHOD tTJSNI_Tween::Invalidate()
{
	inherited::Invalidate();
}
//---------------------------------------------------------------------------
double TJS_INTF_METHOD tTJSNI_Tween::calc(int t,int now,
						double start,double end,int duration)

{
	auto f = get_ease_func(t);
	return f(now,start,end,duration);
	// if(t==0 || t==1)
	// {
	// 	return do_ease_liner(now,start,end,duration);
	// }
    // auto tw = tweeny::from(start).to(end).during(duration);//via(tweeny::easing::linear);
    // auto v = via_method(tw,tweeny::easing::quadraticOut);
    // auto v = via_by_type(tw,t);
    // printf("==tween calc:%d,%d,%f,%f,%d\n",t,now,start,end,duration);
    // return v.peek((uint32_t)now);
	// return a*a;
}

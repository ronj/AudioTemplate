#ifndef SAMPLERATEOBSERVER_H
#define SAMPLERATEOBSERVER_H

class SamplerateObserver
{
public:
	virtual ~SamplerateObserver() {}
	virtual void samplerateChanged(float aNewRate, float aOldRate) = 0;
};

#endif // SAMPLERATEOBSERVER_H

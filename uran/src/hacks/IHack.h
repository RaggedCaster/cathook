/*
 * IHack.h
 *
 *  Created on: Oct 5, 2016
 *      Author: nullifiedcat
 */

#ifndef IHACK_H_
#define IHACK_H_

class CUserCmd;

class IHack {
public:
	inline virtual ~IHack() {};
	inline virtual void PaintTraverse(void*, unsigned int, bool, bool) {};
	inline virtual bool CreateMove(void*, float, CUserCmd*) { return true; };
	inline virtual void Create() {};
	inline virtual void Destroy() {};
};

#endif /* IHACK_H_ */
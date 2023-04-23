#ifndef TSMS_MUTABLE_H
#define TSMS_MUTABLE_H

#include "tsms_def.h"

typedef struct TSMS_MUTABLE tMutable;
typedef tMutable * pMutable;

typedef void(*TSMS_MUTABLE_SETTER_CALLBACK)(pMutable, void *, void * handler);
typedef void(*TSMS_MUTABLE_GETTER_CALLBACK)(pMutable, void * handler);

struct TSMS_MUTABLE {
	void * data;
	TSMS_MUTABLE_SETTER_CALLBACK setterCallback;
	TSMS_MUTABLE_GETTER_CALLBACK getterCallback;
	void *setterHandler;
	void *getterHandler;
};

pMutable TSMS_MUTABLE_create(void * data);

TSMS_RESULT TSMS_MUTABLE_set(pMutable mutable, void * data);

void * TSMS_MUTABLE_get(pMutable mutable);

TSMS_RESULT TSMS_MUTABLE_setSetterCallback(pMutable mutable, TSMS_MUTABLE_SETTER_CALLBACK callback, void * handler);

TSMS_RESULT TSMS_MUTABLE_setGetterCallback(pMutable mutable, TSMS_MUTABLE_GETTER_CALLBACK callback, void * handler);

#endif //TSMS_MUTABLE_H

//
//  GUObject.cpp
//  CoreStructures
//

#include "GUObject.h"

namespace CoreStructures {

	GUObject::GUObject() {

		retainCount = 1; // initialise retainCount - calling function adopts ownership
	}


	GUObject::~GUObject() {
	}


	// retain object
	void GUObject::retain() {

		retainCount++;
	}

	// Release ownership of object.  If retainCount=0 then delete object.  Return true if the object is deleted successfully - so calling function knows if pointer/handle to object is valid or not.
	bool GUObject::release() {

		retainCount--;

		if (retainCount==0) {

			delete(this);
			return true;
		}

		return false;
	}


	// Accessor methods

	unsigned int GUObject::getRetainCount() {

		return retainCount;
	}

}

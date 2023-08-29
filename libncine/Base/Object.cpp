#include "Object.h"
#include "../ServiceLocator.h"
#include "../../Common.h"

namespace nCine
{
	Object::Object(ObjectType type)
		: type_(type), id_(0)
	{
		id_ = theServiceLocator().indexer().addObject(this);
	}

	Object::~Object()
	{
		// Don't remove the object id from indexer if this is a moved out object
		if (id_ > 0) {
			theServiceLocator().indexer().removeObject(id_);
		}
	}

	Object::Object(Object&& other) noexcept
		: type_(other.type_), id_(other.id_)
	{
		theServiceLocator().indexer().setObject(id_, this);
		other.id_ = 0;
	}

	Object& Object::operator=(Object&& other) noexcept
	{
		type_ = other.type_;
		theServiceLocator().indexer().removeObject(id_);
		id_ = other.id_;

		other.id_ = 0;
		return *this;
	}

	template<class T>
	T* Object::fromId(unsigned int id)
	{
		const Object* object = theServiceLocator().indexer().object(id);

		if (object != nullptr) {
			if (object->type_ == T::sType()) {
				return static_cast<T*>(object);
			} else { // Cannot cast
				LOGF("Object %u is of type %u instead of %u", id, object->type_, T::sType());
				return nullptr;
			}
		} else {
			LOGW("Object %u not found", id);
			return nullptr;
		}
	}

	Object::Object(const Object& other)
		: type_(other.type_), id_(0)
	{
		id_ = theServiceLocator().indexer().addObject(this);
	}
}

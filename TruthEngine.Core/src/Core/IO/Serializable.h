#pragma once

#define SERIALIZABLE_UNIFIED(Type) class Type; \
namespace boost {\
namespace serialization{\
template<class Archive> void serialize(Archive& ar, Type& obj, const unsigned int version);\
}\
}


#define SERIALIZABLE_UNIFIED1(namespace1, Type)\ 
namespace namespace1{ class Type; } \
namespace boost {\
namespace serialization{\
template<class Archive> void serialize(Archive& ar, namespace1::Type& obj, const unsigned int version);\
}\
}

#define SERIALIZABLE_UNIFIED2(namespace1, namepsace2, Type)\ 
namespace namespace1{ namespace namespace2 { class Type; } } \
	namespace boost {\
		namespace serialization {\
			template<class Archive> void serialize(Archive& ar, namespace1::namespace2::Type& obj, const unsigned int version); \
	}\
}

#define SERIALIZABLE_UNIFIED3(namespace1, namepsace2, namespace3, Type)\
namespace namespace1 { namespace namespace2 { namespace namespace3 { class Type; } } }\
namespace boost {\
	namespace serialization {\
		template<class Archive> void serialize(Archive& ar, namespace1::namespace2::namespace3::Type& obj, const unsigned int version);\
	}\
}


#define SERIALIZABLE_SEPARATE(Type) class Type; \
namespace boost {\
namespace serialization{\
template<class Archive> void save(Archive& ar, const Type& obj, const unsigned int version);\
template<class Archive> void load(Archive& ar, Type& obj, const unsigned int version);\
}\
}\
BOOST_SERIALIZATION_SPLIT_FREE(Type);


#define SERIALIZABLE_SEPARATE1(namespace1, Type)\
namespace namespace1{ class Type; }\
namespace boost {\
namespace serialization{\
template<class Archive> void save(Archive& ar, const namespace1::Type& obj, const unsigned int version);\
template<class Archive> void load(Archive& ar, namespace1::Type& obj, const unsigned int version);\
}\
}\
BOOST_SERIALIZATION_SPLIT_FREE(namespace1::Type);


#define SERIALIZABLE_SEPARATE2(namespace1, namespace2, Type)\
namespace namespace1{namespace namespace2{ class Type; }}\
namespace boost {\
namespace serialization{\
template<class Archive> void save(Archive& ar, const namespace1::namespace2::Type& obj, const unsigned int version);\
template<class Archive> void load(Archive& ar, namespace1::namespace2::Type& obj, const unsigned int version);\
}\
}\
BOOST_SERIALIZATION_SPLIT_FREE(namespace1::namespce2::Type);


#define SERIALIZABLE_SEPARATE3(namespace1, namespace2, namespace3, Type)\
namespace namespace1{namespace namespace2{namespace namespace3{ class Type; }}}\
namespace boost {\
namespace serialization{\
template<class Archive> void save(Archive& ar, const namespace1::namespace2::namespace3::Type& obj, const unsigned int version);\
template<class Archive> void load(Archive& ar, namespace1::namespace2::namespace3::Type& obj, const unsigned int version);\
}\
}\
BOOST_SERIALIZATION_SPLIT_FREE(namespace1::namespace2::namespace3::Type);


#define SERIALIZE_UNIFIED(Type) template<class Archive> friend void ::boost::serialization::serialize(Archive& ar, Type& obj, const unsigned int version);


#define SERIALIZE_SEPARATE(Type) template<class Archive> friend void ::boost::serialization::save(Archive& ar, const Type& obj, const unsigned int version);\
template<class Archive> friend void ::boost::serialization::load(Archive& ar, Type& obj, const unsigned int version);\

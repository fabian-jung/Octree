#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "Container/CommunicationPattern.hpp"

template <template<class> class Container, template<template <class> class> class Model, template<class, class> class CurveTemplate>
struct ConfigType {
	typedef Model<Container> ModelType;
	typedef typename ModelType::NodeType NodeType;
	typedef typename ModelType::AccessPattern AccessPattern;

	typedef Container<NodeType> MatType;
	typedef CurveTemplate<MatType, AccessPattern> Curve;

	typedef typename MatType::CommunicationPatternMatType CommunicationPatternMatType;
	typedef typename ModelType::CommunicationPatternAccessPattern CommunicationPatternAccessPattern;
	typedef CurveTemplate<CommunicationPatternMatType, CommunicationPatternAccessPattern> CommunicationPatternCurve;
	typedef CommunicationPattern<CommunicationPatternMatType, CommunicationPatternAccessPattern, CommunicationPatternCurve> CommunicationPatternType;
};

#endif

/**
 * riskservice.hpp
 * Defines the data types and Service for fixed income risk.
 *
 * @author Breman Thuraisingham
 */
#ifndef RISK_SERVICE_HPP
#define RISK_SERVICE_HPP

#include "soa.hpp"
#include "positionservice.hpp"

/**
 * PV01 risk.
 * Type T is the product type.
 */
template<typename T>
class PV01
{

public:

  // ctor for a PV01 value
	PV01() {}
  PV01(const T &_product, double _pv01, long _quantity);

  // Get the product on this PV01 value
  T& GetProduct()  
  {
	  return product;
  }

  // Get the PV01 value
  double GetPV01() const 
  {
	  return pv01;
  }

  // Get the quantity that this risk value is associated with
  long GetQuantity() const 
  {
	  return quantity;
  }

  void ChangeQuantity(long diff) {
	  quantity = diff;
  }

private:
  T product;
  double pv01;
  long quantity;

};

/**
 * A bucket sector to bucket a group of securities.
 * We can then aggregate bucketed risk to this bucket.
 * Type T is the product type.
 */
template<typename T>
class BucketedSector
{

public:

  // ctor for a bucket sector
  BucketedSector(){}
  BucketedSector(const vector<T> &_products, string _name);

  // Get the products associated with this bucket
   vector<T>& GetProducts() ;

  // Get the name of the bucket
   string& GetName() ;

private:
  vector<T> products;
  string name;

};

/**
 * Risk Service to vend out risk for a particular security and across a risk bucketed sector.
 * Keyed on product identifier.
 * Type T is the product type.
 */
template<typename T>
class RiskService : public Service<string,PV01 <T> >
{

public:

  // Add a position that the service will risk
  void AddPosition(Position<T> &position);

  // Get the bucketed risk for the bucket sector
  const PV01< BucketedSector<T> >& GetBucketedRisk(const BucketedSector<T> &sector) const;

};

template<typename T>
PV01<T>::PV01(const T &_product, double _pv01, long _quantity) :
  product(_product)
{
  pv01 = _pv01;
  quantity = _quantity;
}

template<typename T>
BucketedSector<T>::BucketedSector(const vector<T>& _products, string _name) :
  products(_products)
{
  name = _name;
}

template<typename T>
 vector<T>& BucketedSector<T>::GetProducts() 
{
  return products;
}

template<typename T>
 string& BucketedSector<T>::GetName() 
{
  return name;
}

#endif

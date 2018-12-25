/**
 * positionservice.hpp
 * Defines the data types and Service for positions.
 *
 * @author Breman Thuraisingham
 */
#ifndef POSITION_SERVICE_HPP
#define POSITION_SERVICE_HPP

#include <string>
#include <map>
#include "soa.hpp"
#include "tradebookingservice.hpp"

using namespace std;

/**
 * Position class in a particular book.
 * Type T is the product type.
 */
template<typename T>
class Position
{

public:

  // ctor for a position
	Position(){}
  Position(const T &_product);

  // Get the product
  const T& GetProduct() const;

  // Get the position quantity
  long GetPosition(string& book);

  // Get the aggregate position
  long GetAggregatePosition();

  void AddPosition(string book,long quantity) 
  {
	  positions[book] += quantity;
  }

  void SubPosition(string book,long quantity) 
  {
	  positions[book] -= quantity;
  }

private:
  T product;
  map<string,long> positions;

};

/**
 * Position Service to manage positions across multiple books and secruties.
 * Keyed on product identifier.
 * Type T is the product type.
 */
template<typename T>
class PositionService : public Service<string,Position <T> >
{

public:

  // Add a trade to the service
  virtual void AddTrade( Trade<T> &trade) = 0;

};

template<typename T>
Position<T>::Position(const T &_product) :
  product(_product)
{
}

template<typename T>
const T& Position<T>::GetProduct() const
{
  return product;
}

template<typename T>
long Position<T>::GetPosition(string& book)
{
  return positions[book];
}

template<typename T>
long Position<T>::GetAggregatePosition()
{
	long AggregatePosition = 0;
	for (auto& position : positions)
	{
		AggregatePosition += position.second;
	}
	return AggregatePosition;
}

#endif

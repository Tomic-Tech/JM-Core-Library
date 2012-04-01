#include "jmdiag.hpp"

namespace JM
{
	Diag::Diag()
	{

	}

	Diag::~Diag()
	{

	}

	Diag& Diag::inst()
	{
		static Diag instance;
		return instance;
	}
}
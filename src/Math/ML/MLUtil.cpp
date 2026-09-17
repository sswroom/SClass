#include "Stdafx.h"
#include "Math/ML/MLUtil.h"

void Math::ML::MLUtil::SetupMat(arma::mat& xMat, NN<Data::ArrayListNN<Data::ArrayListDbl>> x)
{
	UIntOS i = 0;
	UIntOS j = x->GetCount();
	if (j == 0)
	{
		xMat.set_size(0, 0);
		return;
	}
	UIntOS k = x->GetItemNoCheck(0)->GetCount();
	xMat.set_size(k, j);
	while (i < j)
	{
		UIntOS l = 0;
		while (l < k)
		{
			xMat(l, i) = x->GetItemNoCheck(i)->GetItem(l);
			l++;
		}
		i++;
	}
}

void Math::ML::MLUtil::SetupMatInv(arma::mat& xMat, NN<Data::ArrayListNN<Data::ArrayListDbl>> x)
{
	UIntOS i = 0;
	UIntOS j = x->GetCount();
	if (j == 0)
	{
		xMat.set_size(0, 0);
		return;
	}
	UIntOS k = x->GetItemNoCheck(0)->GetCount();
	xMat.set_size(j, k);
	while (i < j)
	{
		UIntOS l = 0;
		while (l < k)
		{
			xMat(i, l) = x->GetItemNoCheck(i)->GetItem(l);
			l++;
		}
		i++;
	}
}

void Math::ML::MLUtil::SetupCubeInv(arma::cube& xCube, NN<Data::ArrayListNN<Data::ArrayListDbl>> x)
{
	UIntOS i = 0;
	UIntOS j = x->GetCount();
	if (j == 0)
	{
		xCube.set_size(0, 0, 0);
		return;
	}
	UIntOS k = x->GetItemNoCheck(0)->GetCount();
	xCube.set_size(j, 1, k);
	while (i < j)
	{
		UIntOS l = 0;
		while (l < k)
		{
			xCube(i, 0, l) = x->GetItemNoCheck(i)->GetItem(l);
			l++;
		}
		i++;
	}
}
void Math::ML::MLUtil::SetupLastRow(arma::cube& yCube, UIntOS batchSize, NN<Data::ArrayListDbl> y)
{
	UIntOS i = 0;
	UIntOS j = y->GetCount();
	if (j == 0)
	{
		yCube.set_size(0, 0, 0);
		return;
	}
	yCube.set_size(batchSize, 1, j);
	yCube.zeros();
	while (i < j)
	{
		yCube(batchSize - 1, 0, i) = y->GetItem(i);
		i++;
	}
}

void Math::ML::MLUtil::SetupRowVec(arma::rowvec& yVec, NN<Data::ArrayListDbl> y)
{
	UIntOS i = 0;
	UIntOS j = y->GetCount();
	yVec.set_size(j);
	while (i < j)
	{
		yVec[i] = y->GetItem(i);
		i++;
	}
}

NN<Data::ArrayListDbl> Math::ML::MLUtil::FromRowVec(arma::rowvec& yVec)
{
	NN<Data::ArrayListDbl> result;
	NEW_CLASSNN(result, Data::ArrayListDbl(yVec.n_elem));
	UIntOS i = 0;
	UIntOS j = yVec.n_elem;
	while (i < j)
	{
		result->Add(yVec[i]);
		i++;
	}
	return result;
}

NN<Data::ArrayListDbl> Math::ML::MLUtil::FromLastRow(arma::cube& yCube)
{
	NN<Data::ArrayListDbl> result;
	NEW_CLASSNN(result, Data::ArrayListDbl(yCube.n_slices));
	UIntOS i = 0;
	UIntOS j = yCube.n_slices;
	while (i < j)
	{
		result->Add(yCube(yCube.n_rows - 1, 0, i));
		i++;
	}
	return result;
}

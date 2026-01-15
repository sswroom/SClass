#include "Stdafx.h"
#include "Data/ML/Keras.h"
#include <fdeep/fdeep.hpp>

class KerasModel : public Data::ML::MLModel
{
private:
	fdeep::model model;
public:
	KerasModel(fdeep::model &model) : model(model)
	{
	}

	virtual ~KerasModel()
	{
	}

	virtual void PredictMulti(NN<Data::ArrayListArr<UnsafeArray<Double>>> historyData, UOSInt historyStep, NN<Data::ArrayListArr<Double>> predictData)
	{
		std::vector<fdeep::float_type> d;
		UnsafeArray<UnsafeArray<Double>> currD;
		UOSInt k;
		UOSInt i = 0;
		UOSInt j = historyData->GetCount();
		while (i < j)
		{
			d.clear();
			currD = historyData->GetItemNoCheck(i);
			k = 0;
			while (k < historyStep)
			{
				d.push_back((fdeep::float_type)currD[k][0]);
				k++;
			}
			fdeep::tensors result = this->model.predict({fdeep::tensor(fdeep::tensor_shape((std::size_t)historyStep, (std::size_t)1), d)});
			fdeep::tensor resultT = result.at(0);
			fdeep::internal::float_vec_unaligned vec = resultT.to_vector();
			UnsafeArray<Double> r = MemAllocArr(Double, (UOSInt)vec.size());
			k = (UOSInt)vec.size();
			while (k-- > 0)
			{
				r[k] = vec.at((std::size_t)k);
			}
			predictData->Add(r);
			i++;
		}
	}

	virtual void Predict(UnsafeArray<UnsafeArray<Double>> historyData, UOSInt historyStep, NN<Data::ArrayListNative<Double>> predictData)
	{
		std::vector<fdeep::float_type> d;
		UOSInt i;
		UOSInt j;
		i = 0;
		while (i < historyStep)
		{
			d.push_back((fdeep::float_type)historyData[i][0]);
			i++;
		}
		fdeep::tensors result = this->model.predict({fdeep::tensor(fdeep::tensor_shape((std::size_t)historyStep, (std::size_t)1), d)});
		fdeep::tensor resultT = result.at(0);
		fdeep::internal::float_vec_unaligned vec = resultT.to_vector();
		j = (UOSInt)vec.size();
		i = 0;
		while (i < j)
		{
			predictData->Add(vec.at((std::size_t)i));
			i++;
		}
	}
};

NN<IO::LogTool> Keras_log;

void Keras_logger(std::string s)
{
	Keras_log->LogMessage(Text::CStringNN((const UTF8Char*)s.c_str(), s.length()), IO::LogHandler::LogLevel::Action);
}

Optional<Data::ML::MLModel> Data::ML::Keras::LoadModel(Text::CStringNN fileName, NN<IO::LogTool> log)
{
	try
	{
		Keras_log = log;
		fdeep::model model = fdeep::load_model(std::string((const Char*)fileName.v.Ptr()), true, Keras_logger);
		NN<Data::ML::MLModel> ret;
		NEW_CLASSNN(ret, KerasModel(model));
		return ret;
	}
	catch(const std::exception& e)
	{
		log->LogMessage(Text::CStringNN::FromPtr((const UTF8Char*)e.what()), IO::LogHandler::LogLevel::Error);
		log->LogStackTrace(IO::LogHandler::LogLevel::ErrorDetail);
	}
	return nullptr;
}

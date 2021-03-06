#include "Estimator.h"

Estimator::Estimator()
{
	measurementList = NULL;
	input = NULL;
	filterManager = NULL;
	reset();
}

Estimator::~Estimator()
{
	destructor();	
}
void Estimator::destructor()
{
	if (measurementList != NULL)
		delete measurementList;
	measurementList = NULL;
	
	if (input != NULL)
		delete input;
	input = NULL;

	if (filterManager != NULL)
		delete filterManager;
	filterManager = NULL;
}

void Estimator::reset()
{
	destructor();

	prevLocation = Vector();
}


void Estimator::setEstimator(EstimatorArgument *estimatorArgument)
{
	reset();
	
	args = estimatorArgument;

	args->applyPlanes();		// make reflected beacons list
	
	args->setCutThreshold();

	setMeasurementList();
	setSolver();
	setInput();
	setFilterManager();

	analyzer = &args->analyzer;

}


void Estimator::setMeasurementList()
{

	measurementList = new MeasurementList(args->lid, &args->beacons, &args->planes);


	MeasurementCondition measurementCondition;

	measurementCondition.minSize = args->minBeaconSize;
	measurementCondition.validSize = args->validSize;
	measurementCondition.strictValidSize = args->strictValidSize;
	measurementCondition.timeWindow = args->timeWindow;

	measurementCondition.shortDistanceFirst = (bool)
			!(args->estimatorMode == EST::TRADITIONAL || (!args->optimization & OPT::SELECTION));
//	measurementCondition.shortDistanceFirst = true;
	measurementCondition.smallerNVSSFirst = (bool)(args->optimization & OPT::SELECTION);

	measurementCondition.minValidDistance = args->minValidDistance;
	measurementCondition.random = &args->random;

	
	measurementList->setMeasurementCondition(measurementCondition);

	

}

void Estimator::setSolver()
{
	SolverCondition condition;

	condition.solveNaive = (bool)(args->estimatorMode == EST::TRADITIONAL);

	condition.cutBranch1 = (args->optimization & OPT::BRANCHCUT) != 0;
	condition.cutBranch2 = (args->optimization & OPT::BRANCHCUT_2) != 0;

	condition.maxMeasError = args->maxMeasError;
	
	condition.minBeaconSize = args->minBeaconSize;

	condition.analyzer = &args->analyzer;

	solver.setSolverCondition(condition);
	multiSolver.setSolverCondition(condition);
}

void Estimator::setInput()
{
	input = new SolverInput(measurementList);
}

void Estimator::setFilterManager()
{
	KFArgument kfArgs;
	kfArgs.mode = args->kfMode;
	kfArgs.timeSlot = args->timeSlot;
	kfArgs.KFMeasError = args->kfMeasError;
	kfArgs.KFSystemError = args->kfSystemError;
	filterManager = new FilterManager(kfArgs);
}


void Estimator::measure(unsigned long timestamp, int userBid, double distance)
{
	measurementList->measure(userBid, timestamp, distance);
}


void Estimator::optimize1(SolverResultList *results)
{
	results->nOverThreshold = 0;
	results->nOutside = 0;

	SolverResult *result;
	double sqrThreshold = pow(args->cutThreshold, 2);
	for (size_t i = 0; i < results->size(); i++)
	{
		result = results->at(i);
		if (result->getError() > sqrThreshold)
		{
			result->overThreshold = true;
			results->nOverThreshold ++;
		}

		if (!args->planes.checkInside(result->location))
		{
			result->isInside = false;
			results->nOutside ++;
		}
	}
	
}

EstimatorResult Estimator::solve(long currentTime)
{
	/**/analyzer->N_tick.addValue(1.0);										// N_tick
	/**/analyzer->estimatorTotal.startTimer();								// T1 start
	/**/analyzer->estimatorSetup.startTimer();								// <--- T2 start

	input->setup(currentTime, prevLocation);
	SolverResultList results;
	EstimatorResult prevResult(prevLocation, input->getError(prevLocation));

	/**/analyzer->estimatorSetup.stopTimer();								// ---> T2 end

	/**/analyzer->N_selected.addValue(input->measurements.size());			// ---- V1

	/**/analyzer->estimatorSolving.startTimer();							// <--- T3 start


	solver.solve(input, &results);
//	multiSolver.solve(input, &results);

#if 0
	//debug
	printf("nPMS = %d\n", results.validSize());
#endif

	/**/analyzer->estimatorSolving.stopTimer();								// ---> T3 end

	if (results.isFail())
	{
		// number of beacons are too small

		/**/analyzer->N_selectionFail.addValue(1.0);						// ---- V2
		/**/analyzer->N_fail.addValue(1.0);

		/**/analyzer->estimatorTotal.stopTimer();							// T1 stop
		prevResult.receptionLevel = 2;
		return prevResult;
	}

	/**/analyzer->N_PMS.addValue((double)results.validSize());				// ----- V3


	if (args->optimization & OPT::THRESHOLD)
	{
		//cut threshold and check inside
		/**/analyzer->estimatorPostProc.startTimer();						// <--- T4 start

		optimize1(&results);
		
		/**/analyzer->estimatorPostProc.stopTimer();						// ---> T4 end

		/**/analyzer->N_PMSFiltered.addValue((double)results.validSize());	// ---- V4

		if (results.validSize() == 0)
		{
			analyzer->N_optThresholdFail.addValue(1.0);						// ---- V5
		}
	}

#if 0
	//debug code
	for (size_t i = 0; i < results.size(); i++)
	{
		SolverResult *result = results.at(i);
		result->location.print();
		if (!result->isValid())
			printf("!");
		printf("\n");
	}
#endif

	EstimatorResult ret;
	SolverResult result;

	switch(args->estimatorMode)
	{
	case EST::KFONLY:
	case EST::PROPOSED1:

		analyzer->N_PMS_KF.addValue((double)results.validSize());
		/**/analyzer->estimatorKFProc.startTimer();							// <--- T5 start

		filterManager->getCorrectedResult(&results);
		result = results.getFilteredResult();

		/**/analyzer->estimatorKFProc.stopTimer();							// ---> T5 end
		
		if (!result.isValid())
		{
			// there is no valid result
			ret =  prevResult;
			analyzer->N_kalmanFilterFail.addValue(1.0);						// ---- V5
			analyzer->N_fail.addValue(1.0);
		}
		else
		{
			ret.location = result.getCorrectedLocation();
			ret.error = result.getCorrectedError();
		}
		break;

	case EST::TRADITIONAL:

		result = results.getFirstResult();
			
		if (result.isValid())
		{
			ret.location = result.location;
			ret.error = result.error;
		}
		else
		{
			//the first result is not valid

			analyzer->N_fail.addValue(1.0);
			ret = prevResult;
		}
		break;
	
	default:
		printf("unknown esimator mode. %d\n", args->estimatorMode);
		exit(20);
		break;
	}

	prevLocation = ret.location;
	
	/**/analyzer->estimatorTotal.stopTimer();	// T1 end
	/**///analyzer->tickTimeSlot();

	if (input->measurements.size() < (size_t)args->minBeaconSize)
		ret.receptionLevel = 2;
	else if (input->measurements.size() < (size_t)args->validSize)  	
		ret.receptionLevel = 1;
	else
		ret.receptionLevel = 0;
	

	return ret;

}










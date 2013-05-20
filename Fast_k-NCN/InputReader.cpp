#include "InputReader.h"

namespace Utility {

	InputReader::InputReader() : isStandardizationEnabled(false) {}

	InputReader::~InputReader() {}

	bool InputReader::readInput(int argc, char** argv) {
		try {
			po::options_description generic("Generic options");
			generic.add_options()
				("help", "produce help message")
				("properties-file,p", po::value<std::string>(&propertiesFilename)->implicit_value(""),
				"provide filepath to file with properties")
				("cross-validate,C", po::value<std::string>(&toCrossValidateFilename)->implicit_value(""),	"10-fold cross-validate provided dataset")
				("standardize,S", "standardize data in provided training and testing datasets")
				;

			po::options_description config("Configuration options");
			config.add_options()
				("result-file,r", po::value<std::string>(&resultFilename)->implicit_value("Results/results_knn.txt"),
				"provide filepath to file where results should be stored")
				("training-file,t", po::value<std::string>(&trainFilename)->implicit_value("Datasets/ftrain01.txt"),
				"provide filepath to file with training samples data")
				("testing-file,T", po::value<std::string>(&testFilename)->implicit_value("Datasets/ftest01.txt"),
				"provide filepath to file with testing samples data")
				("classifier,c", po::value<std::string>(&classifierName)->implicit_value("Sequential_kNN"),
				"provide name of classifier to use")
				("number-nn,k", po::value<int>(&k)->default_value(1),
				"provide number of neares (centroid) neighbours")
				("largest-k-to-check,l", po::value<int>(&largestK)->default_value(0),
				"provide number of neares (centroid) neighbours")
				("nr-load-train-samples", po::value<int>(&nrLoadTrainSamples)->default_value(0),
				"provide number of training samples to load, default (0) - all")
				("nr-load-test-samples", po::value<int>(&nrLoadTestSamples)->default_value(0),
				"provide number of testing samples to load, default (0) - all")
				("nr-load-sample-dims", po::value<int>(&nrLoadSampleDims)->default_value(0),
				"provide number of dimensions for each sample to load, default (0) - all")
				("threshold", po::value<int>(&threshold)->default_value(-1),
				"provide threshold dimension for premature termination, default (-1) - all")
				("percentmaxrobustrank", po::value<float>(&percentMaxRobustRank)->default_value(95.0),
				"provide percent of samples from training set for mRobustRank calculation in LimitedV1_kNCN and LimitedV2_kNCN, default (95.0) - all")
				("nrSamplesInBlock", po::value<int>(&nrSamplesInBlock)->default_value(-1),
				"provide number of samples in block for CacheEfficient_kNCN, default (-1) - all samples")
				;

			po::options_description cmdline_options;
			cmdline_options.add(generic).add(config);

			po::options_description config_file_options;
			config_file_options.add(config);

			po::options_description visible("Allowed options");
			visible.add(generic).add(config);

			po::positional_options_description p;
			p.add("properties-file", -1);

			po::store(po::command_line_parser(argc, argv).
				options(cmdline_options).positional(p).run(), vars);
			po::notify(vars);

			if (vars.count("help")) {
				std::cout << visible;
				exit(0);
			}

			if (vars.count("cross-validate")) {
				std::cout << "File with data to cross-validate: " << toCrossValidateFilename  + ".txt" << std::endl;
				std::ifstream toCrossValdiateFile(toCrossValidateFilename + ".txt");
				if (!toCrossValdiateFile.is_open()) {
					std::cerr << "File with data to cross-validate does not exist." << std::endl;
					return false;
				}
				return true;
			}

			if (vars.count("properties-file")) {
				std::ifstream propertiesFile(propertiesFilename);
				if (!propertiesFile.is_open()) {
					std::cerr << "can not open config file: " << propertiesFilename << std::endl;
					vars.clear();
					std::cerr << "Reading properties unsuccesful." << std::endl;
					return false;
				} else {
					store(parse_config_file(propertiesFile, config_file_options), vars);
					notify(vars);
				}
			}

			// Check file with training samples
			if (vars.count("training-file")) {
				std::cout << "Training file: " << trainFilename << std::endl;
				std::ifstream trainFile(trainFilename);
				if (!trainFile.is_open()) {
					std::cerr << "File with training samples data does not exist." << std::endl;
					return false;
				}
			}

			// Check file with testing samples
			if (vars.count("testing-file")) {
				std::cout << "Testing file: " << testFilename << std::endl;
				std::ifstream testFile(testFilename);
				if (!testFile.is_open()) {
					std::cerr << "File with test samples data does not exist" << std::endl;
					return false;
				}
			}

			std::cout << "Results file: " << resultFilename << std::endl;

			if (vars.count("standardize")) {
				isStandardizationEnabled = true;
			}

			EnumParser<ClassifierType> parser;
			classifier = parser.ParseEnum(classifierName);

			// Number of k Nearest Neighbours
			if (k < 1) {
				std::cerr << "Number of k Nearest Neighbours must be larger than or equal to 1" << std::endl;
				return false;
			}

			// Number of training samples to read
			if (nrLoadTrainSamples < 0) {
				std::cerr << "Number of training samples to load must be larger than 0. (Default: 0 == all)" << std::endl;
				return false;
			}

			// Number of testing samples to read
			if (nrLoadTestSamples < 0) {
				std::cerr << "Number of test samples to read must be larger than 0. (Default: 0 == all)" << std::endl;
				return false;
			}
			// Number of dimensions to read
			if (nrLoadSampleDims < 0) {
				std::cerr << "Number of test sample dims to read must be larger than 0. (Default: 0 == all)" << std::endl;
				return false;
			}
		} catch (std::exception& e) {
			std::cerr << e.what() << std::endl;
			vars.clear();
			std::cerr << "Reading properties unsuccesful." << std::endl;
			return false;
		}

		std::cout << boost::format("Reading properties succesful.") << std::endl;
		return true;
	}
}
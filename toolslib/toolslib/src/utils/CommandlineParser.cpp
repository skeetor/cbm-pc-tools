/*******************************************************************************
 *
 * ToolsLib (c) by Gerhard W. Gruber in 2014
 *
 ******************************************************************************/

#include <iostream>
#include <exception>

#include "toolslib/utils/CommandlineParser.h"

using namespace std;

namespace toolslib
{
	namespace utils
	{
		CommandlineParser::CommandlineParser(bool strict)
		: mStrict(strict)
		{
			mOptions.emplace_back(Option().name("").arguments((uint32_t)-1, (uint32_t)-1));
		}

		CommandlineParser::~CommandlineParser()
		{
		}

		uint32_t CommandlineParser::getErrorIndex() const
		{
			return mErrorIndex;
		}

		const string& CommandlineParser::getErrorParam() const
		{
			return mErrorParam;
		}

		const vector<CommandlineParser::Option>& CommandlineParser::getOptions() const
		{
			return mOptions;
		}

		void CommandlineParser::help() const
		{
			if (!getHeader().empty())
				cout << getHeader() << endl;

			for (const Option &o : mOptions)
			{
				if (o.param().empty() && o.name().empty())
					cout << o.name() << " / " << o.param();
				else if (o.param().empty())
					cout << o.param();
				else
					cout << o.name();

				cout << ":";

				if (o.isMandatory())
					cout << " M";
				else
					cout << " O";

				if (o.minArguments() != -1 || o.maxArguments() != -1)
				{
					if (o.minArguments() == -1)
						cout << " * ... " << to_string(o.maxArguments());
					else if (o.maxArguments() == -1)
						cout << o.minArguments() << " ... N";
					else
						cout << o.minArguments() << " ... " << o.maxArguments();
				}

				cout << " " << o.description();

				cout << endl;
			}
		}

		bool CommandlineParser::hasArgument(const string oName) const
		{
			return !getArgument(oName).empty();
		}

		const CommandlineParser::Option *CommandlineParser::findParam(const string& oName) const
		{
			if (oName.empty())
				return nullptr;

			for (const Option& o : getOptions())
			{
				if (oName == o.param())
					return &o;
			}

			return nullptr;
		}

		const CommandlineParser::Option *CommandlineParser::findName(const string& oName) const
		{
			if (oName.empty())
				return nullptr;

			for (const Option& o : getOptions())
			{
				if (oName == o.name())
					return &o;
			}

			return nullptr;
		}

		const CommandlineParser::Option &CommandlineParser::getUnnamed() const
		{
			return mOptions[0];
		}

		const vector<vector<string>> &CommandlineParser::getArguments(const string& oName) const
		{
			const Option *o = findName(oName);
			if (o == nullptr)
			{
				o = findParam(oName);
				if (o == nullptr)
					throw invalid_argument("Name does not exist!");
			}

			return o->values();
		}

		const vector<string> &CommandlineParser::getArgument(const string& oName, uint32_t nIndex) const
		{
			const Option *o = findName(oName);
			if (o == nullptr)
				o = findParam(oName);

			const vector<vector<string>>& values = o->values();

			if (o == nullptr || values.empty() || nIndex >= values.size())
				throw invalid_argument("Name does not exist!");

			return values[nIndex];
		}

		CommandlineParser::Option& CommandlineParser::addOption(const Option& oOption)
		{
			// One of them must be set.
			if (oOption.name().empty() && oOption.param().empty())
				throw invalid_argument("Name and Param may not both be empty!");

			if (findName(oOption.name()) != nullptr)		// Already exists
				throw invalid_argument("Name already exists!");

			if (findParam(oOption.param()) != nullptr)		// Already exists
				throw invalid_argument("Param already exists!");

			mOptions.emplace_back(oOption);

			return mOptions.back();
		}

		CommandlineParser::Option& CommandlineParser::addOption(const string& oOption, const string& oParam, const string& oDescription)
		{
			if (oOption.empty() && oParam.empty())
				throw invalid_argument(oOption + " may not be empty!");

			Option o;
			o.name(oOption);
			o.param(oParam);
			o.description(oDescription);
			return addOption(o);
		}

		bool CommandlineParser::isParam(const std::string& param) const
		{
			if (param.empty())
				return false;

			return param[0] == '-';
		}

		bool CommandlineParser::validateOptionParamCount(const Option& oOption, uint32_t nAdditionals) const
		{
			const vector<vector<string>>& values = oOption.values();

			// Option doesn't allow multiple occurences
			if (values.size() > 1 && oOption.isSingle())
				return false;

			int32_t minargs = oOption.minArguments();
			int32_t maxargs = oOption.maxArguments();

			// This option allows any number of parameters 0..N, so it is always ok.
			if (minargs == -1 && maxargs == -1)
				return true;

			for (vector<string> params : values)
			{
				int size = (int)params.size();
				if (minargs != -1 && size < minargs)
					return false;

				if (maxargs != -1 && size > maxargs)
					return false;
			}

			// If we have additionals we also check if the last section contains the correct numbers if the
			// additionals were added because this is what the command line would look like
			// I.E. --only_one_argument valid_argument wrong_argument --next_option
			// would mean that we have one additional here which would be invalid.
			if (nAdditionals > 0)
			{
				const vector<string>& params = values.back();
				int size = (int)params.size() + nAdditionals;
				if (minargs != -1 && size < minargs)
					return false;

				if (maxargs != -1 && size > maxargs)
					return false;
			}

			return true;
		}

		bool CommandlineParser::parse(int argc, char *argv[])
		{
			std::vector<std::string> args;
			args.reserve(argc - 1);

			for (int i = 1; i < argc; i++)
			{
				if(argv[i])
					args.push_back(argv[i]);
				else
					args.push_back("");
			}

			return parse(args);
		}

		void CommandlineParser::reset()
		{
			mErrorIndex = (uint32_t)-1;
			mErrorParam = "";

			for (Option &o : mOptions)
				o.reset();
		}

		bool CommandlineParser::parse(const vector<string>& args)
		{
			reset();

			if (args.empty())
				return false;

			Option *undefined = const_cast<Option *>(&getUnnamed());		// unknown params go here.
			undefined->addSection();
			Option *current = undefined;
			Option *prev = undefined;
			int arg_count = 0;

			for (size_t i = 0; i < args.size(); i++)
			{
				const string& param = args[i];

				// If this is a passthrough parameter, we always store the args without parsing.
				if (current->isPassThrough())
				{
					current->getLatest().emplace_back(param);
					continue;
				}

				if (isParam(param))
				{
					string name;
					Option *o = nullptr;

					if (param.size() > 1 && param[1] == '-')
					{
						name = param.substr(2);
						if (name.length() == 0)
							name = "--";

						o = const_cast<Option *>(findName(name));
					}
					else
					{
						name = param.substr(1);
						if (name.length() == 0)
							name = "-";

						o = const_cast<Option *>(findParam(name));
					}

					// Validate the option we are leaving before we switch to a new one.
					if (o == nullptr || !validateOptionParamCount(*current, 0) || !validateOptionParamCount(*prev, arg_count))
					{
						mErrorIndex = (uint32_t)i;
						mErrorParam = param;
						return false;
					}

					arg_count = 0;
					prev = undefined;
					current = o;
					current->addSection();
					continue;
				}

				uint32_t max = current->maxArguments();
				if (max != -1 && current->getLatest().size() >= max)
				{
					arg_count = 0;
					prev = current;
					current = undefined;
				}

				// Count how many arguments we would have passed to the previous option
				if (current->name().empty())
					arg_count++;

				current->getLatest().emplace_back(param);

				if ((current == undefined && isStrict()) || !validateOptionParamCount(*current, arg_count))
				{
					mErrorIndex = (uint32_t)i;
					mErrorParam = param;
					return false;
				}
			}

			arg_count = -1;
			for (const Option &o : mOptions)
			{
				arg_count++;
				if (arg_count == 0)
					continue;

				const vector<vector<string>> &values = o.values();
				if (values.size() == 0 && o.isMandatory())
				{
					mErrorIndex = (uint32_t)arg_count;
					if (!o.name().empty())
						mErrorParam = o.name();
					else
						mErrorParam = o.param();

					return false;
				}

				if (!validateOptionParamCount(o, 0u))
				{
					mErrorIndex = (uint32_t)arg_count;
					if (!o.name().empty())
						mErrorParam = o.name();
					else
						mErrorParam = o.param();

					return false;
				}
			}

			return true;
		}
	}
}

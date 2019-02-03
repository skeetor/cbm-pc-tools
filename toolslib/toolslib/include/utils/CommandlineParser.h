/*******************************************************************************
 *
 * ToolsLib (c) by Gerhard W. Gruber in 2014
 *
 ******************************************************************************/

#ifndef COMMANDLINE_PARSER_INCLUDED_H
#define COMMANDLINE_PARSER_INCLUDED_H

#include "toolslib_api.h"

#include <string>
#include <vector>

namespace toolslib
{
	namespace utils
	{
		class TOOLSLIB_API CommandlineParser
		{
		public:
			class TOOLSLIB_API Option
			{
				friend CommandlineParser;

			public:
				virtual ~Option()
				{
				}

				bool undefined() const
				{
					return mParam.empty() && mName.empty() && mValues.empty();
				}

				bool empty() const
				{
					return mValues.empty();
				}

				Option& addValue(const std::string& oValue)
				{
					getLatest().emplace_back(oValue);
					return *this;
				}

				const std::vector<std::vector<std::string>>& values() const
				{
					return mValues;
				}

				Option name(const std::string& oName)
				{
					mName = oName;
					return *this;
				}

				const std::string& name() const
				{
					return mName;
				}

				Option& param(std::string oParam)
				{
					mParam = oParam;
					return *this;
				}

				const std::string& param() const
				{
					return mParam;
				}

				Option& description(const std::string& oDescription)
				{
					mDescription = oDescription;
					return *this;
				}

				const std::string& description() const
				{
					return mDescription;
				}

				Option& single()
				{
					mSingle = true;
					return *this;
				}

				Option& multiple()
				{
					mSingle = false;
					return *this;
				}

				Option& passThrough(bool bPassThrough)
				{
					mPassThrough = bPassThrough;
					return *this;
				}

				bool isPassThrough() const
				{
					return mPassThrough;
				}

				bool isSingle() const
				{
					return mSingle;
				}

				bool isMultiple() const
				{
					return !mSingle;
				}

				Option& optional()
				{
					mOptional = true;
					return *this;
				}

				Option& mandatory()
				{
					mOptional = false;
					return *this;
				}

				bool isOptional() const
				{
					return mOptional;
				}

				bool isMandatory() const
				{
					return !mOptional;
				}

				Option& arguments()
				{
					mMinArgs = 1;
					mMaxArgs = 1;
					return *this;
				}

				Option& arguments(uint32_t nArguments)
				{
					mMinArgs = nArguments;
					mMaxArgs = nArguments;
					return *this;
				}

				Option& arguments(uint32_t nMinArguments, uint32_t nMaxArguments)
				{
					mMinArgs = nMinArguments;
					mMaxArgs = nMaxArguments;
					return *this;
				}

				Option& minArguments(uint32_t nMinArgs)
				{
					mMinArgs = nMinArgs;
					return *this;
				}

				Option& maxArguments(uint32_t nMaxArgs)
				{
					mMaxArgs = nMaxArgs;
					return *this;
				}

				uint32_t minArguments() const
				{
					return mMinArgs;
				}

				uint32_t maxArguments() const
				{
					return mMaxArgs;
				}

			protected:
				Option()
				{
					mSingle = true;
					mOptional = true;
					mMinArgs = 0;
					mMaxArgs = 0;
					mPassThrough = false;
				}

				/**
				 * Clears all values.
				 */
				void reset()
				{
					mValues.clear();
				}

				/**
				 * If a parameter can occur multiple times, we need to keep track of each arguments individually
				 * @return
				 */
				std::vector<std::string>& addSection()
				{
					// The first entry contains all items which are not specified with a switch
					mValues.emplace_back(std::vector<std::string>());
					return mValues.back();
				}

			private:
				std::vector<std::string>& getLatest()
				{
					if (mValues.empty())
						return addSection();

					return mValues.back();
				}

				private:
					uint32_t mMinArgs;
					uint32_t mMaxArgs;
					bool mOptional:1;
					bool mSingle:1;					// Option may appear only once. Additional occurrences overwrite previous ones.
					bool mPassThrough:1;			// If PassThrough is true, this means that commandline parsing is stopped and 
													// all arguments after it, are stored as is.

					std::string mName;				// Long param used with '--'
					std::string mParam;				// Short param used with '-'
					std::string mDescription;
					std::vector<std::vector<std::string>> mValues;
			};

		public:
			/**
			 * If strict == false, arguments which are unknown will be stored as unnamed arguments.
			 * This can be used when mixing paramters with names and without names.
			 */
			CommandlineParser(bool strict = true);
			virtual ~CommandlineParser();

			/**
			 * Clears all currently parsed parameters.
			 */
			void reset();

			/**
			 * Parse the arguments from the commandline parameters
			 */
			bool parse(const std::vector<std::string>& argv);
			bool parse(int argc, char *argv[]);

			/**
			 * If an error was reported this returnes the index of the parameter that caused it.
			 */
			uint32_t getErrorIndex() const;

			/**
			 * Returns the name of the parameter causing the error.
			 */
			const std::string& getErrorParam() const;

			/**
			 * Prints the help description to stdout in the default implementation.
			 */
			virtual void help() const;

			/**
			 * Returns true if the paramter was used on the commandline
			 */
			bool hasArgument(const std::string oName) const;

			/**
			 * Returns the arguments for the unnamed parameters
			 */
			const Option &getUnnamed() const;

			/**
			 * Return the values for the specfied parameter. If the list is empty, the parameter
			 * was not used. 
			 * nIndex specfies the nth usage of the paramter. This is only relevant if the same
			 * parameter can occur multiple times on the commandline.
			 */
			const std::vector<std::string> &getArgument(const std::string& oName, uint32_t nIndex = 0) const;

			/**
			 * Returns the list of values for the specfied parameters. If the list is empty, the
			 * parameter was not used.
			 */
			const std::vector<std::vector<std::string>> &getArguments(const std::string& oName) const;

			/**
			 * Adds the specified option to the parser. If an option with the same name already
			 * exists, an invalid_argument eception is thrown.
			 */
			Option& addOption(const std::string& oOption, const std::string& oParam, const std::string& oDescription);

			/**
			 * Adds the specified option to the parser. If an option with the same name already
			 * exists, an invalid_argument eception is thrown.
			 */
			Option& addOption(const Option& oOption);

		protected:
			bool isStrict() const { return mStrict; }
			void setStrict(bool strict = true) { mStrict = strict; }

			const std::vector<Option>& getOptions() const;
			const Option *findName(const std::string& oName) const;
			const Option *findParam(const std::string& oName) const;
			bool validateOptionParamCount(const Option& oOption, uint32_t nAdditionals) const;
			bool isParam(const std::string& param) const;

		private:
			std::vector<Option> mOptions;
			bool mStrict : 1;
			uint32_t mErrorIndex;
			std::string mErrorParam;
		};
	}
}

#endif // COMMANDLINE_PARSER_INCLUDED_H

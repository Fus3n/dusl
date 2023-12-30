#pragma once

#include <iostream>
#include <string_view>
#include "Position.hpp"

namespace flang {
	enum ErrorType {
		Error,
		SyntaxError,
		RunTimeError,
        NameError,
        ZeroDivisionError,
        UnimplementedError,
        IndexError
	};
	
	std::string errTypeToString(ErrorType err_type);

	class FError {
	public:
		// TODO: add support for multiple positions so it can generate a traceback
		FError(
			ErrorType err_type, 
			std::string_view msg,
			const Position& pos
		) : m_msg(msg), m_pos(pos), m_err_type(err_type){}
			
		std::string generateErrString(); 
		void Throw();

	private:
		const ErrorType m_err_type;
        std::string_view m_msg;
		Position m_pos;
	};

}
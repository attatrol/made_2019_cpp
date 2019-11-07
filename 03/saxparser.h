#ifndef SAXPARSER_H
#define SAXPARSER_H

#include <exception>
#include <string>

#include "observable.h"

/* Event driven string parser */
class SaxParser {
    private:
        /* Event observables */
        Observable<> m_onStart;
        Observable<> m_onEnd;
        Observable<const std::string&> m_onWordParsed; // std::string_view is better
        Observable<unsigned long> m_onIntParsed;
    public:
        /** 
         * Parse input string.
         * Finite automata that pulls tokens one by one from the lexer until EOL.
         */
        void parse(const std::string& input);
        

        /** Add listener callback on parsing started */
        void addListenerOnParseStart(void (* listener)())
        {
            m_onStart.addListener(listener);
        }
        /** Remove listener callback on parsing started */
        void removeListenerOnParseStart(void (* listener)())
        {
            m_onStart.removeListener(listener);
        }
        /** Add listener callback on parsing ended */
        void addListenerOnParseEnd(void (* listener)())
        {
            m_onEnd.addListener(listener);
        }
        /** Remove listener callback on parsing ended */
        void removeListenerOnParseEnd(void (* listener)())
        {
            m_onEnd.removeListener(listener);
        }
        /** Add listener callback on word encountered */
        void addListenerOnWordParsed(void (* listener)(const std::string&))
        {
            m_onWordParsed.addListener(listener);
        }
        /** Remove listener callback on word encountered  */
        void removeListenerOnWordParsed(void (* listener)(const std::string&))
        {
            m_onWordParsed.removeListener(listener);
        }
        /** Add listener callback on parsing integer encountered  */
        void addListenerOnIntParsed(void (* listener)(unsigned long))
        {
            m_onIntParsed.addListener(listener);
        }
        /** Remove listener callback on parsing integer encountered  */
        void removeListenerOnIntParsed(void (* listener)(unsigned long))
        {
            m_onIntParsed.removeListener(listener);
        }
        
    /* Possible error codes of the parser */
    enum ErrorCode {
        INPUT_OVERFLOW = 0,       /* Read number is beyond domain of unsigned long type */
        TOKENS_NOT_SEPARATED = 1  /* Unseparated word and number encountered */
    };

    /* Object wrapper over ErrorCode */
    struct ParserException : public std::exception {
        const ErrorCode errorCode;
    
        ParserException(ErrorCode errorCode): errorCode(errorCode) { }
    
    	const char* what() noexcept 
    	{
            return "Parser exception";
    	}
	};
};


#endif /* SAXPARSER_H */

#include <functional>
#include <iostream>
#include <sstream>
#include <vector>
#include <utility>

#include "saxparser.h"

/* basic tests for the Observable helper class */

int counter = 0;
void listener1 ()
{
    counter += 1;
    counter *= 3;
}
void listener2()
{
    counter += 1;
    counter *= 5;
}
bool testObservable()
{
    Observable<> observable;
    
    // simple execution
    observable.addListener(listener1);
    observable.addListener(listener2);
    observable.fireEvent();
    if (counter != 20)
    {
        return false;
    }
    observable.removeListener(listener1);
    observable.removeListener(listener2);
    observable.fireEvent();
    if (counter != 20)
    {
        return false;
    }
    
    // simple execution
    counter = 0;
    observable.addListener(listener2);
    observable.addListener(listener1);
    observable.fireEvent();
    if (counter != 18)
    {
        return false;
    }
    observable.removeListener(listener1);
    observable.removeListener(listener2);
    observable.fireEvent();
    if (counter != 18)
    {
        return false;
    }
    
    // double add and remove
    counter = 0;
    observable.addListener(listener2);
    observable.addListener(listener2);
    observable.fireEvent();
    if (counter != 5)
    {
        return false;
    }
    observable.removeListener(listener2);
    observable.removeListener(listener2);
    observable.fireEvent();
    if (counter != 5)
    {
        return false;
    }
    
    // double add doesn't change order
    counter = 0;
    observable.addListener(listener2);
    observable.addListener(listener1);
    observable.addListener(listener2);
    observable.fireEvent();
    if (counter != 18)
    {
        return false;
    }
    observable.removeListener(listener2);
    observable.removeListener(listener1);
    observable.fireEvent();
    if (counter != 18)
    {
        return false;
    }
    return true;
}

struct Word
{
    const unsigned int idx;
    const std::string value;
};

struct Integer
{
    const unsigned int idx;
    const unsigned long value;
};


/** A test case for the parser run test */
struct ExpressionTestCase
{
    const std::string name;                /* case name */
    const std::string input;               /* input string */
    const std::vector<Integer> integers;   /* expected integers */
    const std::vector<Word> words;         /* expected words */
    const bool success;                    /* expected outcome */
    const SaxParser::ErrorCode errorCode; /* expected error code if !success */
};

/* Collection of various expressions */
const  ExpressionTestCase FIXTURES[] =
{
    { 
        "Empty input-0", "", 
        {},
        {},
        true, SaxParser::ErrorCode::INPUT_OVERFLOW
    },
    { 
        "Empty input-1", "   ", 
        {},
        {},
        true, SaxParser::ErrorCode::INPUT_OVERFLOW
    },
    {
        "Single int-0", "0", 
        {
            { 0, 0 }
        }, 
        {}, 
        true, SaxParser::ErrorCode::INPUT_OVERFLOW 
    },
    {
        "Single int-1", "  42", 
        {
            { 0, 42 }
        }, 
        {}, 
        true, SaxParser::ErrorCode::INPUT_OVERFLOW  
    },
    {
        "Single int-2", "232323  ", 
        {
            { 0, 232323 }
        }, 
        {}, 
        true, SaxParser::ErrorCode::INPUT_OVERFLOW  
    },
    {
        "Single word-0", "Hello", 
        {}, 
        {
            { 0, "Hello" }
        }, 
        true, SaxParser::ErrorCode::INPUT_OVERFLOW  
    },
    {
        "Single word-1", "   World!", 
        {}, 
        {
            { 0, "World!" }
        }, 
        true, SaxParser::ErrorCode::INPUT_OVERFLOW  
    },
    {
        "Single word-2", "YES   ", 
        {}, 
        {
            { 0, "YES" }
        }, 
        true, SaxParser::ErrorCode::INPUT_OVERFLOW  
    },
    {
        "Int + word-0", "144 square", 
        {
            { 0, 144 }
        },
        {
            { 1, "square" }
        }, 
        true, SaxParser::ErrorCode::INPUT_OVERFLOW  
    },
    {
        "Int + word-1", "    300 spartans", 
        {
            { 0, 300 }
        },
        {
            { 1, "spartans" }
        }, 
        true, SaxParser::ErrorCode::INPUT_OVERFLOW  
    },
    {
        "Int + word-2", "100000     roses       ", 
        {
            { 0, 100000 }
        },
        {
            { 1, "roses" }
        }, 
        true, SaxParser::ErrorCode::INPUT_OVERFLOW  
    },
    {
        "Word + int-0", "Than 0", 
        {
            { 1, 0 }
        },
        {
            { 0, "Than" }
        }, 
        true, SaxParser::ErrorCode::INPUT_OVERFLOW  
    },
    {
        "Word + int-1", "   Reached      1000000000     ", 
        {
            { 1, 1000000000 }
        },
        {
            { 0, "Reached" }
        }, 
        true, SaxParser::ErrorCode::INPUT_OVERFLOW  
    },
    {
        "Word + int-2", "Got 4     ", 
        {
            { 1, 4 }
        },
        {
            { 0, "Got" }
        }, 
        true, SaxParser::ErrorCode::INPUT_OVERFLOW  
    },
    {
        "Word + int-2", "Got 4     ", 
        {
            { 1, 4 }
        },
        {
            { 0, "Got" }
        }, 
        true, SaxParser::ErrorCode::INPUT_OVERFLOW  
    },
    {
        "max + 1", "18446744073709551616", 
        { },
        { }, 
        false, SaxParser::ErrorCode::INPUT_OVERFLOW  
    },
    {
        "max+10", "18446744073709551625    ", 
        {
            { 1, 4 }
        },
        {
            { 0, "Got" }
        }, 
        false, SaxParser::ErrorCode::INPUT_OVERFLOW  
    },
    {
        "max", "    18446744073709551615", 
        {
            { 0, 18446744073709551615UL }
        },
        {}, 
        true, SaxParser::ErrorCode::INPUT_OVERFLOW  
    },
    {
        "max-1", "    18446744073709551614", 
        {
            { 0, 18446744073709551614UL }
        },
        {}, 
        true, SaxParser::ErrorCode::INPUT_OVERFLOW  
    },
    {
        "max-10", "    18446744073709551605", 
        {
            { 0, 18446744073709551605UL }
        },
        {}, 
        true, SaxParser::ErrorCode::INPUT_OVERFLOW  
    },
    {
        "Syntax error-0", "abba0", 
        {},
        {}, 
        false, SaxParser::ErrorCode::TOKENS_NOT_SEPARATED  
    },
    {
        "Syntax error-1", "0x11111111", 
        {},
        {}, 
        false, SaxParser::ErrorCode::TOKENS_NOT_SEPARATED  
    },
    {
        "Syntax error-2", "1111111DD", 
        {},
        {}, 
        false, SaxParser::ErrorCode::TOKENS_NOT_SEPARATED  
    },
    {
        "Complex-0", "The priest had a dog", 
        {},
        {
            { 0, "The" }, { 1, "priest" }, { 2, "had" }, { 3, "a" }, { 4, "dog" }
        }, 
        true, SaxParser::ErrorCode::TOKENS_NOT_SEPARATED  
    },
    {
        "Complex-1", "You can't divide by 10 another 0", 
        {
            { 4, 10 }, { 6, 0 }
        },
        {
            { 0, "You" }, { 1, "can't" }, { 2, "divide" }, { 3, "by" }, { 5, "another" }
        }, 
        true, SaxParser::ErrorCode::TOKENS_NOT_SEPARATED  
    },
    {
        "Complex-2", "1000000000 is great, but 1000000000000 is even better", 
        {
            { 0, 1000000000 }, { 4, 1000000000000 }
        },
        {
            { 1, "is" }, { 2, "great," }, { 3, "but" }, { 5, "is" }, { 6, "even" }, { 7, "better" }
        }, 
        true, SaxParser::ErrorCode::TOKENS_NOT_SEPARATED  
    },
    
};

/* Test state, essentually a closure for holding test results */
struct TestState {
    bool onStartFired;
    bool onEndFired;
    unsigned int tokenCount;
    const std::vector<Word>* words;
    bool wrongWord;
    const std::vector<Integer>* integers;
    bool wrongInteger;
    
    void clear()
    {
        tokenCount = 0;
        onStartFired = onEndFired = wrongInteger = wrongWord = false;
        words = nullptr;
        integers = nullptr;
    }
    
    bool isValid()
    {
        return onStartFired && onEndFired && !wrongWord && !wrongInteger && tokenCount == words->size() + integers->size();
    }
};

TestState testState;

void onStart()
{
    testState.onStartFired = true;
}

void onEnd()
{
    testState.onEndFired = true;
}

void onWordParsed(const std::string& word)
{
    for(auto it = testState.words->begin(); it != testState.words->end(); ++it)
    {
        if((*it).value == word && (*it).idx == testState.tokenCount)
        {
            testState.tokenCount++;
            return;
        }
    }
    testState.wrongWord = true;
    return;
}

void onIntegerParsed(unsigned long value)
{
    for(auto it = testState.integers->begin(); it != testState.integers->end(); ++it)
    {
        if((*it).value == value && (*it).idx == testState.tokenCount)
        {
            testState.tokenCount++;
            return;
        }
    }
    testState.wrongInteger = true;
    return;
}

/** Test an expression parsing */
bool runExpressionTest(SaxParser& parser, const ExpressionTestCase& fixture)
{
    testState.clear();
    testState.words = &fixture.words;
    testState.integers = &fixture.integers;
    try 
    {
        parser.parse(fixture.input);
        return fixture.success && testState.isValid();
    } 
    catch (const SaxParser::ParserException& e)
    {
        return !fixture.success && e.errorCode == fixture.errorCode;
    }
}

/** Test suit */
void runTests() {
    std::cout << "Test run started." << std::endl;
    if (!testObservable())
        std::cout << "Observable template test failed" << std::endl;
    
    TestState testState;
    
    SaxParser parser;
    parser.addListenerOnParseStart(onStart);
    parser.addListenerOnParseEnd(onEnd);
    parser.addListenerOnWordParsed(onWordParsed);
    parser.addListenerOnIntParsed(onIntegerParsed);

    for (ExpressionTestCase fixture : FIXTURES)
        if(!runExpressionTest(parser, fixture))
            std::cout << "Expression test \"" << fixture.name << "\" failed." << std::endl;
    std::cout << "Test run completed." << std::endl;
}

/** Program entry point */
int main(void) {
    runTests();
}

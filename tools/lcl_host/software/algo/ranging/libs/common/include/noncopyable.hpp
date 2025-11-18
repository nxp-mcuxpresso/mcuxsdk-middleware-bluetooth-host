/***********************************************************************************/
/*!
 *  @brief      
 *  @file       noncopyable.h
 *  @author     Martijn Hijdra (martijn.hijdra@imec-nl.nl)
 *  @version    GIT
 *  @copyright  Stichting Imec Nederland (http://www.imec-nl.nl)
 *  @copyright  *** IMEC STRICTLY CONFIDENTIAL ***
 *
 *  @note       
 */
/***********************************************************************************/

#ifndef NONCOPYABLE_HPP_
#define NONCOPYABLE_HPP_

/*
 * Example of make a class explicitly non-copyable
 *
 * class CantCopy : private NonCopyable { }
*/

class NonCopyable {
public:
	NonCopyable(const NonCopyable&) = delete;
	NonCopyable& operator =(const NonCopyable&) = delete;

protected:
	NonCopyable() = default;
	~NonCopyable() = default; /// Protected non-virtual destructor
};

#endif /* NONCOPYABLE_HPP_ */

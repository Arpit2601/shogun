/*
 * This software is distributed under BSD 3-clause license (see LICENSE file).
 *
 * Authors: Giovanni De Toni, Soeren Sonnenburg, Thoralf Klein, Viktor Gal
 */

#include <csignal>
#include <stdlib.h>

#include <rxcpp/rx-lite.hpp>
#include <shogun/io/SGIO.h>
#include <shogun/lib/Signal.h>

using namespace shogun;
using namespace rxcpp;

bool CSignal::m_active = true;
CSignal::SGSubjectS* CSignal::m_subject = new rxcpp::subjects::subject<int>();

CSignal::SGObservableS* CSignal::m_observable =
    new CSignal::SGObservableS(CSignal::m_subject->get_observable());
CSignal::SGSubscriberS* CSignal::m_subscriber =
    new CSignal::SGSubscriberS(CSignal::m_subject->get_subscriber());

CSignal::CSignal()
{
}

CSignal::~CSignal()
{
}

void CSignal::handler(int signal)
{
	/* If the handler is not enabled exit */
	if (!m_active)
		exit(-1);

	if (signal == SIGINT)
	{
		io::print(
		    "\n[ShogunSignalHandler] "
		    "Immediately return to prompt / "
		    "Prematurely finish computations / "
		    "Pause current computation / "
		    "Do nothing (I/C/P/D)? ");
		char answer = getchar();
		getchar();
		switch (answer)
		{
		case 'I':
			io::print("[ShogunSignalHandler] Killing the application...\n");
			m_subscriber->on_completed();
			exit(0);
			break;
		case 'C':
			io::print(
			    "[ShogunSignalHandler] Terminating"
			    " prematurely current algorithm...\n");
			m_subscriber->on_next(SG_BLOCK_COMP);
			break;
		case 'P':
			io::print("[ShogunSignalHandler] Pausing current computation...");
			m_subscriber->on_next(SG_PAUSE_COMP);
			break;
		default:
			io::print("[ShogunSignalHandler] Continuing...\n");
			break;
		}
	}
	else
	{
		io::print("[ShogunSignalHandler] Unknown signal {} received\n", signal);
	}
}

void CSignal::reset_handler()
{
	delete m_subject;
	delete m_observable;
	delete m_subscriber;

	m_subject = new rxcpp::subjects::subject<int>();
	m_observable = new CSignal::SGObservableS(m_subject->get_observable());
	m_subscriber = new CSignal::SGSubscriberS(m_subject->get_subscriber());
}

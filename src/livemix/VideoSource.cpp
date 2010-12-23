#include "VideoSource.h"

#include "VideoWidget.h"

//////////////

//#ifndef UINT64_C
//#define UINT64_C(val) val##ui64

// 7.18.4.2 Macros for greatest-width integer constants
//#define INTMAX_C   INT64_C
//#define UINTMAX_C  UINT64_C

#ifndef INT64_C
#define INT64_C(c) (c ## LL)
#define UINT64_C(c) (c ## ULL)
#endif



extern "C" {
#include "libswscale/swscale.h"
#include "libavdevice/avdevice.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
}

bool VideoSource::isLibAVInit = false;

void VideoSource::initAV()
{
	if(isLibAVInit)
		return;
	isLibAVInit = true;
	
	//qDebug() << "VideoSource::initAV()";
	
	avcodec_init();
	avcodec_register_all();
	avdevice_register_all();
	av_register_all();
}

//////////////

VideoSource::VideoSource(QObject *parent)
	: QThread(parent)
	, m_killed(false)
	, m_isBuffered(true)
	, m_singleFrame(0)
{
	m_frameQueue.setMaxByteSize(1024 * 1024 //  1 MB
				         *   64 // 64 MB
				   );

	initAV();
}

void VideoSource::registerConsumer(QObject *consumer)
{
	m_consumerList.append(consumer);
	connect(consumer, SIGNAL(destroyed()), this, SLOT(consumerDestroyed()));
	//qDebug() << "VideoSource::registerConsumer(): "<<this<<": consumer list:"<<m_consumerList.size(); //m_refCount:"<<m_refCount;
}

VideoSource::~VideoSource() 
{
	if(!m_killed)
		destroySource();
}

void VideoSource::run()
{
	exec();
}

VideoFrame *VideoSource::frame()
{
	QMutexLocker lock(&m_queueMutex);
	if(!m_isBuffered ||
	    m_frameQueue.isEmpty())
	{
		#ifdef DEBUG_VIDEOFRAME_POINTERS
		qDebug() << "VideoSource::frame(): Testing validity of m_singleFrame:"<<m_singleFrame;
		#endif
		
		if(m_singleFrame)
		{
			#ifdef DEBUG_VIDEOFRAME_POINTERS
			qDebug() << "VideoSource::frame(): Calling incRef() on m_singleFrame:"<<m_singleFrame;
			#endif
			
			m_singleFrame->incRef();
		}
		#ifdef DEBUG_VIDEOFRAME_POINTERS
		qDebug() << "VideoSource::frame(): Returning m_singleFrame:"<<m_singleFrame;
		#endif
		
		return m_singleFrame;
	}
	//qDebug() << "VideoSource::frame(): Queue size: "<<m_frameQueue.size();
	VideoFrame *frame = m_frameQueue.dequeue();
	if(frame)
	{
		/// NB: Don't need to incRef here because we called incRef() when adding to the queue
		//qDebug() << "VideoSource::frame(): Calling incRef() on frame from queue:"<<frame;
		//frame->incRef();
	}
	#ifdef DEBUG_VIDEOFRAME_POINTERS
	qDebug() << "VideoSource::frame(): Returning frame from queue:"<<frame;
	#endif
	
	return frame;
}

void VideoSource::enqueue(VideoFrame *frame)
{
	//QMutexLocker lock(&m_queueMutex);
	m_queueMutex.lock();
	if(m_isBuffered)
	{
		// incRef() here instead of when we take from the queue because if we dont, then the singleFrame code 
		// below will try to release this frame and delete it when the next frame comes in.
		#ifdef DEBUG_VIDEOFRAME_POINTERS
		qDebug() << "VideoSource::enqueue(): Calling incRef() on frame going into queue:"<<frame;
		#endif
		
		frame->incRef();
		m_frameQueue.enqueue(frame);
	}
	//else
	if(m_singleFrame)
	{
		#ifdef DEBUG_VIDEOFRAME_POINTERS
		qDebug() << "VideoSource::enqueue(): Releasing old m_singleFrame:"<<m_singleFrame;
		#endif
		
		if(m_singleFrame->release())
		{
			#ifdef DEBUG_VIDEOFRAME_POINTERS
			qDebug() << "VideoSource::enqueue(): Deleting old m_singleFrame:"<<m_singleFrame;
			#endif
			
			delete m_singleFrame;
			m_singleFrame = 0;
		}
	}
	m_singleFrame = frame;
	
	#ifdef DEBUG_VIDEOFRAME_POINTERS
	qDebug() << "VideoSource::enqueue(): Calling incRef() on m_singleFrame:"<<m_singleFrame;
	#endif
	
	m_singleFrame->incRef();
	
 	//qDebug() << "VideoSource::enqueue(): "<<this<<" m_isBuffered:"<<m_isBuffered<<", Queue size: "<<m_frameQueue.size();
	m_queueMutex.unlock();
	
	emit frameReady();
}

void VideoSource::setIsBuffered(bool flag)
{
	m_isBuffered = flag;
	if(!flag)
		m_frameQueue.clear();
}

void VideoSource::consumerDestroyed()
{
	release(sender());
}

void VideoSource::release(QObject *consumer)
{
	if(!consumer)
		return;
		
	if(!m_consumerList.contains(consumer))
		return;
		
	m_consumerList.removeAll(consumer);
	
	//m_refCount --;
	//qDebug() << "VideoSource::release(): "<<this<<": consumer list:"<<m_consumerList.size(); //m_refCount:"<<m_refCount;
	//if(m_refCount <= 0)
	if(m_consumerList.isEmpty())
	{
		destroySource();
	}
}

void VideoSource::destroySource()
{
	//qDebug() << "VideoSource::destroySource(): "<<this;
	m_killed = true;
	quit();
	wait();
	deleteLater();
}

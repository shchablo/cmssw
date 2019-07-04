/*!
\file 
\brief Header file with definitions of iRPCMap class.
\authors Shchablo Konstantin (shchablo@gmail.com)
\version 1.0
\copyright Copyright 2019 Shchablo Konstantin.
\license This file is released under the GNU General Public License v3.0.
\date May 2019
*/

#ifndef RecoLocalMuon_iRPCInfo_h
#define RecoLocalMuon_iRPCInfo_h

/* std */
#include <map>

/*!
	\brief This class defines an association map and parameters for the cluster algorithm for improved Resistive Plate Chamber (iRPC). 
	\author Shchablo 
	\version 1.0
	\date May 2019
*/
class iRPCInfo
{
  public:
    
 		/*
			\brief Constructor.
		*/
		iRPCInfo();
		/*!
			\brief Destructor.
		*/
		~iRPCInfo();
    
    /*!
		  \brief Checking that TDC channel related to the high radius. 
			\param[in] channel - TDC channel number.
			\param[out] strip - PCB strip number.
      \return True if the channel is connected to high radius.
		*/
    bool isHR(int channel, int &strip);
    /*!
		  \brief Checking that TDC channel related to the low radius. 
			\param[in] channel - TDC channel number.
			\param[out] strip - PCB strip number.
      \return True if the channel is connected to low radius.
		*/
    bool isLR(int channel, int &strip);
    
    /*!
		  \brief Set dependency map between the low radius and TDC channels. 
			\param[in] map - std::map<channel, strip>.
		*/
    void setMapHR(std::map<int, int> &map);
    /*!
		  \brief Set dependency map between the high radius and TDC channels. 
			\param[in] map - std::map<channel, strip>.
		*/
    void setMapLR(std::map<int, int> &map);
    
    /*!
		  \brief Get dependency map between the high radius and TDC channels. 
			\return map - std::map<channel, strip>.
		*/
    std::map<int, int> getMapHR();
    /*!
		  \brief Get dependency map between the low radius and TDC channels. 
			\return map - std::map<channel, strip>.
		*/
    std::map<int, int> getMapLR();
    
    /*!
		  \brief Get time threshold for clustering by time for high radius. 
			\return _thrTimeHR.
		*/
    float thrTimeHR();
    /*!
		  \brief Get time threshold for clustering by time for low radius. 
			\return _thrTimeLR.
		*/
    float thrTimeLR();
    
    /*!
		  \brief Set time threshold for clustering by time for high radius. 
			\param[in] _thrTimeHR.
		*/
    void setThrTimeHR(float thrTime);
    /*!
		  \brief Set time threshold for clustering by time for low radius. 
			\param[in] _thrTimeLR.
		*/
    void setThrTimeLR(float thrTime);
    
    /*!
		  \brief Get minimum time threshold for associations clusters from the high and low radius. Depends on chamber geometry. 
			\return _thrDeltaTimeMin.
		*/
    float thrDeltaTimeMin();
    /*!
		  \brief Get maximum time threshold for associations clusters from the high and low radius. Depends on chamber geometry.
			\return _thrDeltaTimeMax.
		*/
    float thrDeltaTimeMax();
    
    /*!
		  \brief Set minimum time threshold for associations clusters from the high and low radius. Depends on chamber geometry. 
			\return _thrDeltaTimeMin.
		*/
    void setThrDeltaTimeMin(float thrTime);
    /*!
		  \brief Set maximum time threshold for associations clusters from the high and low radius. Depends on chamber geometry.
			\param[in] _thrDeltaTimeMax.
		*/
    void setThrDeltaTimeMax(float thrTime);

    /*!
		  \brief Get the speed of light in a strip. 
			\return _speed.
		*/
    float speed();
    /*!
		  \brief Set the speed of light in a strip. 
			\parm[in] _speed.
		*/
    void setSpeed(float _speed);
    
    /*!
		  \brief Get type of output clusters. If true - only associated clusters. If false - associated&singleSide clusters. 
			\return _isAND.
		*/
    bool isAND();
    /*!
		  \brief Set type of output clusters. If true - only associated clusters. If false - associated&singleSide clusters. 
			\param[in] is - true or false..
		*/
    void isAND(bool is);
    
    /*!
		  \brief Return true if need to use iRPC algorithm. 
			\return _isUse.
		*/
    bool isUse();
    /*!
		  \brief Set true if need to use iRPC algorithm. 
			\param[in] is - true or false.
		*/
    void isUse(bool is);

	private:
    std::map<int, int> _HR; // !< map<channel, strip> for high radius.
    std::map<int, int> _LR; // !< map<channel, strip> for low radius.

    float _thrTimeHR; //  !< Time threshold for clustering by time for high radius.
    float _thrTimeLR; // !< time threshold for clustering by time for low radius
    
    float _thrDeltaTimeMin; // !< Maximum time threshold for associations clusters.
    float _thrDeltaTimeMax; // !< Minimum time threshold for associations clusters.
    
    float _speed; // !< Speed of light in a strip.
    
    bool _isAND; // !< Type of output clusters. If true - only associated clusters. If false - associated&singleSide clusters. 
    bool _isUse; // !< Type of algo. 
};
#endif // RecoLocalMuon_iRPCInfo_h 

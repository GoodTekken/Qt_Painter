//
// Created by asphox on 09/10/18.
//

#include "lidar/TIM561.h"

bool TIM561::connect(const std::string& ip, int port)
{
    /*create TCP socket*/
    m_socketId = socket(AF_INET,SOCK_STREAM,IPPROTO_IP);
    m_socketDescriptor.sin_family = AF_INET;
    m_socketDescriptor.sin_port = htons(port);

    /*convert string ip to valide ip address*/
    m_socketDescriptor.sin_addr.s_addr = inet_addr(ip.c_str());

    /*open TCP socket*/
    return ::connect(m_socketId, reinterpret_cast<sockaddr*>(&m_socketDescriptor), sizeof(m_socketDescriptor) ) == 0;
}

bool TIM561::start()
{
    currentDataPoints.reserve(NBR_DATA);
    for(int i = 0; i < NBR_DATA; i++){
        currentDataPoints.emplace_back(std::pair<float, uint16_t>(0,0));
    }

    if( loginAsService() && loginAsClient() )
    {
        if( write(RUN) )
        {
            read(11);
            return buffer[9] == '1';
        }
    }
    return false;
}

void TIM561::update()
{
    if( write(CONTINUOUS_MEASURE) )
    {
        read(19);
        if (buffer[17] == '0' || buffer[17] == '1')
        {
            read();
            std::istringstream stream(buffer);
            std::string tmp;
            currentScan.part.clear();
            /*get all data separated with space char independently */
            while (std::getline(stream, tmp, ' '))
            {
                currentScan.part.push_back(tmp);
            }
            updateDataPoints();
        }
    }
}

void TIM561::updateDataPoints()
{
    if(upsidedown == false)
    {
        float current_angle = -45;
        for( int i = 0 ; i < NBR_DATA ; i++ )
        {
            /*stoul converts hex string to real hex int*/
            currentDataPoints[i].second = std::stoul(currentScan.part[i+TelegramScan::ID::DATA_1],nullptr,16);
            currentDataPoints[i].first = current_angle;
            current_angle+=STEP_ANGLE;
        }
    }
    else
    {
        float current_angle = 225;
        for( int i = 0 ; i < NBR_DATA ; i++ )
        {
            /*stoul converts hex string to real hex int*/
            currentDataPoints[i].second = std::stoul(currentScan.part[i+TelegramScan::ID::DATA_1],nullptr,16);
            currentDataPoints[i].first = current_angle;
            current_angle-=STEP_ANGLE;
        }
    }


//    TelegramScan telegram;
//    telegram.part = currentScan.part;
//    std::cout<<"getDeviceNumber()"<<telegram.getDeviceNumber()<<std::endl;
//    std::cout<<"getSerialNumber()"<<telegram.getSerialNumber()<<std::endl;
//    std::cout<<"getDeviceStatus()"<<telegram.getDeviceStatus()<<std::endl;
//    std::cout<<"getTelegramCounter()"<<telegram.getTelegramCounter()<<std::endl;
//    std::cout<<"getScanCounter()"<<telegram.getScanCounter()<<std::endl;
//    std::cout<<"getTimeSinceStartup()"<<telegram.getTimeSinceStartup()<<std::endl;
//    std::cout<<"getSerialNumber()"<<telegram.getSerialNumber()<<std::endl;
//    std::cout<<"getTimeTransmission()"<<telegram.getTimeTransmission()<<std::endl;
//    std::cout<<"getInputStatus()"<<telegram.getInputStatus()<<std::endl;
//    std::cout<<"getOutputStatus()"<<telegram.getOutputStatus()<<std::endl;
//    std::cout<<"getScanFreq()"<<telegram.getScanFreq()<<std::endl;
//    std::cout<<"getMesFreq()"<<telegram.getMesFreq()<<std::endl;
//    std::cout<<"getCodeOutput()"<<telegram.getCodeOutput()<<std::endl;
//    std::cout<<"getNumOutput()"<<telegram.getNumOutput()<<std::endl;
//    std::cout<<"getConvFactor()"<<telegram.getConvFactor()<<std::endl;
//    std::cout<<"getConvOffset()"<<telegram.getConvOffset()<<std::endl;
//    std::cout<<"getStartAngle()"<<telegram.getStartAngle()<<std::endl;
//    std::cout<<"getScaleFactor()"<<telegram.getScaleFactor()<<std::endl;
//    std::cout<<"getCount()"<<telegram.getCount()<<std::endl;
}

void TIM561::updatemeandate()
{
    int count = 1;

    //Initialization
    if( write(CONTINUOUS_MEASURE) )
    {
        read(19);

        if (buffer[17] == '0' || buffer[17] == '1')
        {
            read();
            std::istringstream stream(buffer);
            std::string tmp;
            currentScan.part.clear();
            /*get all data separated with space char independently */
            int count = 0;
            while (std::getline(stream, tmp, ' '))
            {
                currentScan.part.push_back(tmp);
                count++;
            }

            if(upsidedown == false)
            {
                float current_angle = -45;
                if(currentScan.part.size()==TOTAL_NUMBER)
                {
                    for( int i = 0 ; i < NBR_DATA ; i++ )
                    {
                        /*stoul converts hex string to real hex int*/
                        currentDataPoints[i].second = std::stoul(currentScan.part[i+TelegramScan::ID::DATA_1],nullptr,16);
                        currentDataPoints[i].first = current_angle;
                        current_angle+=STEP_ANGLE;
                    }
                }
            }
            else
            {
                float current_angle = 225;
                if(currentScan.part.size()==TOTAL_NUMBER)
                {
                    for( int i = 0 ; i < NBR_DATA ; i++ )
                    {
                        /*stoul converts hex string to real hex int*/
                        currentDataPoints[i].second = std::stoul(currentScan.part[i+TelegramScan::ID::DATA_1],nullptr,16);
                        currentDataPoints[i].first = current_angle;
                        current_angle-=STEP_ANGLE;
                    }
                }
            }


        }
    }

    int temp_i;
    for(temp_i=0;temp_i<(count-1);temp_i++)
    {
        if( write(CONTINUOUS_MEASURE) )
        {
            read(19);

            if (buffer[17] == '0' || buffer[17] == '1')
            {
                read();
                std::istringstream stream(buffer);
                std::string tmp;
                currentScan.part.clear();
                /*get all data separated with space char independently */
                while (std::getline(stream, tmp, ' '))
                {
                    currentScan.part.push_back(tmp);
                }
                float current_angle = -45;
                if(currentScan.part.size()==TOTAL_NUMBER)
                {
                    for( int i = 0 ; i < NBR_DATA ; i++ )
                    {
                        /*stoul converts hex string to real hex int*/
                        currentDataPoints[i].second += std::stoul(currentScan.part[i+TelegramScan::ID::DATA_1],nullptr,16);
                        currentDataPoints[i].first += current_angle;
                        current_angle+=STEP_ANGLE;
                    }
                }
            }
        }
//        usleep(20000); //20ms
    }



    for(int i = 0; i < NBR_DATA; i++)
    {
        currentDataPoints[i].second = currentDataPoints[i].second/count;
        currentDataPoints[i].first = currentDataPoints[i].first/(float)count;
    }

}

const std::vector<std::pair<float, uint16_t>> * TIM561::getDataPoints() const
{
    return &currentDataPoints;
}

bool TIM561::login( const char id[] , const char pwd[] , uint8_t acces_id )
{
    std::string tmp = LOGIN;
    tmp+=" ";
    tmp+= id;
    tmp+=" ";
    tmp+= pwd;
    if( write(tmp) )
    {
        read(21);
        if( buffer[19] == '1' )
        {
            m_currentAccessMode=acces_id;
            return true;
        }
    }
    return false;
}

bool TIM561::loginAsClient()
{
    bool status;
    if( m_currentAccessMode > 3 )
    {
        if( loginAsService() )
        {
            status = login(CLIENT_ID,CLIENT_PWD,3);
        }
    }
    else
    {
        status = login(CLIENT_ID,CLIENT_PWD,3);
    }
    return status;
}

bool TIM561::loginAsService()
{
    return login(SERVICE_ID,SERVICE_PWD,4);
}

bool TIM561::loginAsMaintenance()
{
    bool status;
    if( m_currentAccessMode > 2 )
    {
        if( loginAsClient() )
        {
            status = login(MAINTENANCE_ID,MAINTENANCE_PWD,2);
        }
    }
    else
    {
        status = login(MAINTENANCE_ID,MAINTENANCE_PWD,2);
    }
    return status;
}


bool TIM561::write(const std::string& msg)
{
    std::string fmsg;
    fmsg+=STX;
    fmsg+=msg;
    fmsg+=ETX;

    /*writes message on TCP socket*/
    ssize_t nbrByte = ::write(m_socketId,fmsg.substr(0,fmsg.length()).c_str(),fmsg.size());

    /*checks if all bytes are sent*/
    return nbrByte == fmsg.size();
}

void TIM561::read(uint16_t nbrBytesRequired)
{
    buffer.clear();

    uint16_t nbrBytes = 0;
    char tmpChar = '\0';

    if( nbrBytesRequired )
    {
        while ( nbrBytes < nbrBytesRequired )
        {
            /*reads character on socket one by one, and add it to the buffer*/
            nbrBytes += ::read(m_socketId,&tmpChar,1);
            buffer+=tmpChar;
        }
    }
    else
    {
        while( tmpChar != 0x03 )
        {
            nbrBytes = ::read(m_socketId,&tmpChar,1);
            if( tmpChar != 0x03 )buffer+=tmpChar;
        }
    }
}

void TIM561::close()
{
    ::close(m_socketId);
}

TIM561::~TIM561()
{
    close();
}

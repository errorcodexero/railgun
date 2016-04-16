#include "network.hpp"

using namespace std;

void *getInAddr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);

}

int sendAll(int _iS, char* _szBuf, int* _iLen) {
	int iTotal = 0;
	int iBytesLeft = *_iLen;
	int iN;

	while (iTotal < *_iLen) {
		iN = send(_iS, _szBuf+iTotal, iBytesLeft, 0);
		if (iN == -1 ) { break; }
		iTotal += iN;
		iBytesLeft -=iN;
	}

	*_iLen = iTotal;

	return iN==-1?-1:0;
}

void sigChildHandler(int s)
{
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}
Client::Client(const char* _szIP, const char* _szPort, int _iSockType, char _szDelimiter = '\n') {
	vector <char* > vszIP;
	//int iPort = 0; //Only a temporary, validation value
	//do some validation
	//vszIP = vszParseString(_szIP, '.');
	//cout << vszIP.size() << "\n";
	//if (vszIP.size() != 3)
		//exitWithError("Not a valid IP", 3);

	//if (sscanf(_szPort, "%d", &iPort) < 1)
	//	exitWithError("Port Number contains no integers", 5);
	//if (iPort > 65535 || iPort < 0)
	//	exitWithError("Port number is not valid", 4);

	//Set up necessary local variables
	this->szPORT = _szPort;
	this->szIP = _szIP;
	this->iSockType = _iSockType;
	this->iNumBytes = 0;
	this->szDelimiter = _szDelimiter;
	this->szBuf[0] = '\0';

	memset(&aiHints, 0, sizeof aiHints);
	aiHints.ai_family = AF_UNSPEC;
	aiHints.ai_socktype = this->iSockType;

	if ((this->iRV = getaddrinfo(this->szIP, this->szPORT, &this->aiHints, &this->aiServInfo)) != 0) {
		fprintf(stderr, "getAddrInfo: %s\n", gai_strerror(iRV));
		//exit(1);
	}

	for(this->aiP = this->aiServInfo; this->aiP != NULL; this->aiP = this->aiP->ai_next) {
	        if ((this->iSockfd = socket(this->aiP->ai_family, this->aiP->ai_socktype,
	        		this->aiP->ai_protocol)) == -1) {
	            perror("client: socket");
	            continue;
	        }
	        if (this->iSockType == SOCK_STREAM) {
	        	if (connect(this->iSockfd, this->aiP->ai_addr, this->aiP->ai_addrlen) == -1) {
	        		close(this->iSockfd);
	            	perror("client: connect");
	            	continue;
	        	}
	        } else if (this->iSockType == SOCK_DGRAM) {
	        	//Doesn't need a special connect statement
	        } else {
	        	exitWithError("Not Valid Socket Type, or Socket Type Not Yet Implemented",6);
	        }
	        break;
	}

	if (this->aiP == NULL) {
	        fprintf(stderr, "client: failed to connect\n");
	        exit(2);
	    }

	inet_ntop(this->aiP->ai_family, getInAddr((struct sockaddr *)this->aiP->ai_addr),
	            this->szS, sizeof this->szS);
	//fcntl(this->iSockfd, F_SETFL, O_NONBLOCK);
	printf("client: connecting to %s\n", this->szS);

	freeaddrinfo(this->aiServInfo);

}

Client::~Client() {
	close(this->iSockfd);

}

bool Client::bRecv() {
	memset(this->szBuf, 0, sizeof this->szBuf);
	unsigned int i = 0;
	char szTmpBuffer[1];
	if (this->iSockfd == -1)
		exit(1);
	if(this->iSockType == SOCK_STREAM) {
		//cout << sizeof this->szBuf << "\n";
		do	{
			if ((this->iNumBytes = recv(this->iSockfd, szTmpBuffer, 1, 0)) == -1 /* && cout << szTmpBuffer*/) {
				return false;
			} else {
				i++;
				//this->szBuf[0] = szTruncateByDelimiter(&this->szBuf[0], this->szDelimiter);
				if (i > sizeof szBuf - 1){
					break;
				}
				//cout << index;
				this->szBuf[i] = szTmpBuffer[0];
				//cout << szTmpBuffer << " "<< this->szBuf[i] << "\n";
				//szTmpBuffer[0] = '\0';
			}

		} while (szTmpBuffer[0] != this->szDelimiter);
		cout << this->szBuf;

		return true;
	} else {
		cout << "Error: Not Stream Socket\n";
		return false;
	}
	return false;
}

bool Client::bSendTo(const char* _szBuf) {
	if(this->iSockType == SOCK_DGRAM) {
		if ((this->iNumBytes = sendto(this->iSockfd, _szBuf, strlen(_szBuf), 0, aiP->ai_addr, aiP->ai_addrlen)) == -1) {
		        perror("talker: sendto");
		        exit(1);
		        return false;
		    }
		else
			return true;
	} else {
		cout << "Error: Not Datagram Socket\n";
		return false;
	}
}

/*bool Client::bNewIP(const char* _szIP) {
#ifdef DEBUG
	cout << "NOTE: This function clears the buffer\n";
#endif
	int i = sizeof this->szBuf;
	memset(this->szBuf, 0, i);
	int x = sizeof this->szIP;
	if(bIsStringIP(_szIP)) {
		memcpy(this->szIP, _szIP, x);

	}
	else
		return false;

	return true;
}*/


char* Client::szGetData() {
	return this->szBuf;
}

Server::Server (const char* _szPort, int _iSockType, int _iBacklog, unsigned int _iMaxClients, bool _bIsPersistant) {

	this->szPORT = _szPort;
	this->iSockType = _iSockType;
	this->iBacklog = _iBacklog;
	this->iMaxClients = _iMaxClients;
	this->iNewfd = 0;
	this->szBuf = '\0';
	this->iNumBytes = 0;
	this->slSinSize = 0;
	this->bIsPersistant = _bIsPersistant;

	memset(&aiHints, 0, sizeof aiHints);
	aiHints.ai_family = AF_UNSPEC;
	aiHints.ai_socktype = iSockType;
	aiHints.ai_flags = AI_PASSIVE;

	if ((iRV = getaddrinfo(NULL, this->szPORT, &aiHints, &aiServInfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(iRV));
		//exit(1);
	}

	for (this->aiP = this->aiServInfo; this->aiP != NULL; this->aiP = this->aiP->ai_next) {

		if ((this->iSockfd = socket(this->aiP->ai_family, this->aiP->ai_socktype, this->aiP->ai_protocol)) == -1) {
			perror("Server: Socket");
			continue;
		}

		if (setsockopt(this->iSockfd, SOL_SOCKET, SO_REUSEADDR, &this->iYes, sizeof(int)) == -1) {
		            perror("setsockopt");
		            exit(1);
		        }

		if (bind(this->iSockfd, this->aiP->ai_addr, this->aiP->ai_addrlen) == -1) {
			close(this->iSockfd);
			perror("Server: Bind");
			continue;
		}

		break;
	}
	if (this->aiP == NULL) {
		fprintf(stderr, "Server: failed to bind socket\n");
		//exit(2);
	}

	freeaddrinfo(aiServInfo);

	if (this->iSockType == SOCK_STREAM) {

		if (listen(this->iSockfd, this->iBacklog) == -1) {

			perror("Listen");
			//exit(1);
		}

		this->sa.sa_handler = sigChildHandler;
		sigemptyset(&this->sa.sa_mask);
		this->sa.sa_flags = SA_RESTART;
		if (sigaction(SIGCHLD, &this->sa, NULL) == -1) {

			perror("sigaction");
			//exit(1);
		}

	} else if (this->iSockType == SOCK_DGRAM) {
		this->slAddrLen = sizeof this->saCliAddr;

	} else {

		exitWithError("Socket type not available", 3);
	}

}

Server::~Server() {
	this->szBuf = (char *) "";
	if (this->iSockfd == -1 && this->iNewfd == -1){
#ifdef DEBUG
		cout << "Socket Closed\n";
#endif
	}
	else{
		if(shutdown(this->iNewfd, 2) == -1 || shutdown(this->iSockfd, 2) == -1) {
			if (this->iSockfd == -1) {
				exitWithError("New socket Failed to close", 1);
			}
			if (this->iNewfd == -1) {
				exitWithError("Socket Failed to close", 1);
			}
		}
	}
}

bool Server::bBroadcast(const char* _szBuf) {
	unsigned int i = 0; //for sake of comparison errors
	//this->szBuf = _szBuf;

	while (i < this->iMaxClients) {
		cout << i << "\n";
		this->slSinSize = sizeof this->saCliAddr;
		this->iNewfd = accept(this->iSockfd, (struct sockaddr *)&this->saCliAddr, &this->slSinSize);

		if(this->iNewfd == -1) {
			return false;
			continue;
		}
		inet_ntop(this->saCliAddr.ss_family, getInAddr((struct sockaddr *)&this->saCliAddr), this->szS, sizeof this->szS);

		if (!fork()) {
			close(this->iSockfd);
			int iBufLen = strlen(this->szBuf);
			if (sendAll(this->iNewfd, (char *)_szBuf, &iBufLen) == -1)
				return false;
			//close(this->iNewfd);
			//exit(0);
		}
		close(this->iNewfd);
		i++;
	}
	return true;
}

bool Server::bListen() {
	if ((this->iNumBytes = recvfrom(this->iSockfd, this->szBuf, sizeof(this->szBuf-1), 0, (struct sockaddr *)&saCliAddr, &slAddrLen)) == -1) {
		perror("recvform");
		return false;
	}
	this->szBuf[this->iNumBytes] = '\0';
	return true;
}

char* Server::szGetData() {
	return this->szBuf;
}

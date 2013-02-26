// Request Test 
// 

#include <Plib-Generic/Generic.hpp>
#include <Plib-Utility/Utility.hpp>
#include <Plib-Text/Text.hpp>
#include <Plib-Threading/Threading.hpp>
#include <Plib-Network/Network.hpp>
//#include <Plib-Network/ConnectInfo.hpp>
//#include <Plib-Network/Syncsock.hpp>

using namespace Plib;
using namespace Plib::Generic;
using namespace Plib::Text;
using namespace Plib::Threading;
using namespace Plib::Network;
using namespace Plib::Utility;
using namespace std;

class NcRequest : public Request
{
public:

	typedef NcRequest		TResp;

	NcRequest( ) 
	{
		m_peerInfo.ConnectTimeOut = 30000;
	}

	NcRequest( const String & address, Uint32 port ) : Request( address, port )
	{
		m_peerInfo.ConnectTimeOut = 30000;
	}

	~NcRequest() {}

	void SetConnectInfo( const String & address, Uint32 port )
	{
		m_peerInfo.Address.DeepCopy( address );
		m_peerInfo.Port = port;
	}

	void AppendBody( const String & body )
	{
		m_packageBuffer.Append( body );
	}

	const NData & generateFullPackage( )
	{
		return m_packageBuffer;
	}
};

int main ( int argc, char * argv[] )
{
	Plib::Threading::SetSignalHandle();

	String _string = "GET /indexp.php HTTP/1.1\r\nHOST : pushchen.com\r\n\r\n";
	//cout << _string << endl;
	PINFO( Plib::Network::LocalHostname() );
	PINFO( Identify::New() );

	// What I want to write:
	/*
	SomeRequest _req(ServerAddress, ServerPort);
	_req.ParameterOne = ValueOne;
	_req.ParameterTwo = ValueTwo;
	_req.Set(Key1, Value1);
	_req.Append(PackageBody);
	_req.Identify = TIMESTAMP();
	Plib::Network::Connection::Success onSuccess( somefunction );

	Plib::Network::Connection::Async( _req, onSuccess, global_onFailed );
	*/

	Reference< NcRequest > _req;
	_req->SetConnectInfo( "pushchen.com", 80 );
	_req->AppendBody( _string );
	TcpConnection::Async( _req );

	// PeerInfo remotePeer;
	// remotePeer.Address = "127.0.0.1";
	// remotePeer.Port = 9090;
	// //PINFO( localPeer );
	// PDUMP( remotePeer );
/*
	{
		TcpSocket so;
		PIF ( so.Connect(_ncReq.RequestPeerInfo()) ) {
			PDUMP( so );
			so.Write( _ncReq.generateFullPackage() );
			NData resp = so.Read( 5000, false, 1600 );
			PIF ( !resp.RefNull() ) {
				PDUMP( resp.Size() );
				// PrintAsHex( resp );
				PDUMP( resp );
			} else {
				so.Close();
			}
		}
	}
*/
	Plib::Threading::WaitForExitSignal();
	return 0;
}

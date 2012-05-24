using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace JM.Diag
{
    interface ICommbox
    {
        bool Open();
        bool Close();
        void SetConnector(ConnectorType cnn);
        ILink<IProtocol> CreateProtocol(ProtocolType type);
    }
}

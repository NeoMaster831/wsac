using System.Diagnostics;
using WsACService.InterProcess.Abstractions;
using WsACService.InterProcess.Models;

namespace WsACService.InterProcess;

public class FrameWriter(ILowLevelWriter writer)
{
    public async Task WriteAsync(Frame frame, CancellationToken ct)
    {
        writer.Write(Consts.Preamble);
        
        var header = new FrameHeader(frame.Signature, frame.DataSize);
        writer.Write(header);

        if (frame.DataSize > 0)
        {
            // Frame.Data should not be null and checked in Frame's ctor
            // An assertion is for if frame is read from somewhere (even if it's not intended)
            Debug.Assert(frame.Data is not null);
            
            await writer.WriteAsync(frame.Data!, frame.DataSize, ct);
        }
    }
}

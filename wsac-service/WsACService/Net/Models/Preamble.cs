using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Text;

namespace WsACService.Net.Models;

[StructLayout(LayoutKind.Explicit, Pack = 1, Size = Size)]
public struct Preamble : IEquatable<Preamble>
{
    private const int Size = 6;

    [FieldOffset(0)] public unsafe fixed byte  Data[Size];
    [FieldOffset(0)] private             int   High;
    [FieldOffset(4)] private             short Low;

    public Preamble(Span<byte> span)
    {
        unsafe
        {
            fixed (byte* pData = Data)
            fixed (byte* pSpan = span)
                Unsafe.CopyBlockUnaligned(pData, pSpan, Size);
        }
    }

    public void Push(byte last)
    {
        unsafe
        {
            for (var i = 0; i < Size - 1; i++)
                Data[i] = Data[i + 1];
            Data[Size - 1] = last;
        }
    }

    public bool Equals(Preamble other)
    {
        unsafe
        {
            for (var i = 0; i < Size; i++)
                if (Data[i] != other.Data[i])
                    return false;
            return true;
        }
    }

    public override bool Equals(object? obj) => obj is Preamble other && Equals(other);

    public static bool operator ==(Preamble left, Preamble right) => left.Equals(right);
    public static bool operator !=(Preamble left, Preamble right) => !(left == right);

    public override int GetHashCode() => High ^ Low;
    
    public override string ToString()
    {
        unsafe
        {
            var builder = new StringBuilder();
            for (var i = 0; i < Size; i++)
            {
                if (i != 0)
                    builder.Append(':');
                builder.Append(Data[i].ToString("X2"));
            }
            return builder.ToString();
        }
    }
}

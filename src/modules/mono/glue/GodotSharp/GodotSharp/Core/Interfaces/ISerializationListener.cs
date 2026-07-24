namespace GnuChan
{
    public interface ISerializationListener
    {
        void OnBeforeSerialize();
        void OnAfterDeserialize();
    }
}

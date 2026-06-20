using XaApi.Models;

namespace XaApi.Services;

public interface IContactService
{
    Task<List<Contact>> GetAllAsync();
    Task CreateAsync(Contact contact);
}

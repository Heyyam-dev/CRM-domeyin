using Microsoft.EntityFrameworkCore;
using XaApi.Data;
using XaApi.Models;

namespace XaApi.Services;

public class ContactService : IContactService
{
    private readonly AppDbContext _db;

    public ContactService(AppDbContext db)
    {
        _db = db;
    }

    public async Task<List<Contact>> GetAllAsync()
        => await _db.Contacts.OrderByDescending(c => c.CreatedAt).ToListAsync();

    public async Task CreateAsync(Contact contact)
    {
        _db.Contacts.Add(contact);
        await _db.SaveChangesAsync();
    }
}

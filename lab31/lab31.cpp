#include <exception>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>

struct Campaign
{
    int Id;
    std::string Name;
    double Budget;
    bool IsActive;
};

class ICampaignRepository
{
public:
    virtual ~ICampaignRepository() = default;
    virtual void Add(const Campaign& campaign) = 0;
    virtual Campaign* GetById(int campaignId) = 0;
    virtual void UpdateStatus(int campaignId, bool isActive) = 0;
    virtual double GetBudget(int campaignId) = 0;
};

class IAnalyticsService
{
public:
    virtual ~IAnalyticsService() = default;
    virtual void TrackEvent(const std::string& eventName, int campaignId) = 0;
};

class CampaignService
{
private:
    ICampaignRepository* campaignRepository;
    IAnalyticsService* analyticsService;

public:
    CampaignService(ICampaignRepository* campaignRepository, IAnalyticsService* analyticsService)
        : campaignRepository(campaignRepository), analyticsService(analyticsService)
    {
    }

    void CreateCampaign(int campaignId, const std::string& campaignName, double budget)
    {
        if (campaignId <= 0)
        {
            throw std::out_of_range("campaign id must be greater than zero");
        }

        if (campaignName.empty())
        {
            throw std::invalid_argument("campaign name cannot be empty");
        }

        if (budget <= 0.0)
        {
            throw std::out_of_range("budget must be greater than zero");
        }

        Campaign campaign{campaignId, campaignName, budget, false};
        campaignRepository->Add(campaign);
        analyticsService->TrackEvent("campaign_created", campaignId);
    }

    bool ActivateCampaign(int campaignId)
    {
        Campaign* campaign = campaignRepository->GetById(campaignId);

        if (campaign == nullptr)
        {
            return false;
        }

        campaignRepository->UpdateStatus(campaignId, true);
        analyticsService->TrackEvent("campaign_activated", campaignId);
        return true;
    }

    double CalculateRoi(int campaignId, double revenue)
    {
        if (revenue < 0.0)
        {
            throw std::out_of_range("revenue cannot be negative");
        }

        double budget = campaignRepository->GetBudget(campaignId);

        if (budget <= 0.0)
        {
            throw std::invalid_argument("campaign budget must be positive");
        }

        analyticsService->TrackEvent("roi_calculated", campaignId);
        return (revenue - budget) / budget;
    }
};

class MockCampaignRepository : public ICampaignRepository
{
public:
    bool AddCalled = false;
    int AddCallCount = 0;
    Campaign LastAddedCampaign{0, "", 0.0, false};

    bool GetByIdCalled = false;
    int GetByIdCallCount = 0;
    int LastGetByIdId = 0;
    Campaign* GetByIdResult = nullptr;

    bool UpdateStatusCalled = false;
    int UpdateStatusCallCount = 0;
    int LastUpdateStatusId = 0;
    bool LastUpdateStatusValue = false;

    bool GetBudgetCalled = false;
    int GetBudgetCallCount = 0;
    int LastGetBudgetId = 0;
    double GetBudgetResult = 0.0;

    void Add(const Campaign& campaign) override
    {
        AddCalled = true;
        ++AddCallCount;
        LastAddedCampaign = campaign;
    }

    Campaign* GetById(int campaignId) override
    {
        GetByIdCalled = true;
        ++GetByIdCallCount;
        LastGetByIdId = campaignId;
        return GetByIdResult;
    }

    void UpdateStatus(int campaignId, bool isActive) override
    {
        UpdateStatusCalled = true;
        ++UpdateStatusCallCount;
        LastUpdateStatusId = campaignId;
        LastUpdateStatusValue = isActive;
    }

    double GetBudget(int campaignId) override
    {
        GetBudgetCalled = true;
        ++GetBudgetCallCount;
        LastGetBudgetId = campaignId;
        return GetBudgetResult;
    }
};

class MockAnalyticsService : public IAnalyticsService
{
public:
    bool TrackEventCalled = false;
    int TrackEventCallCount = 0;
    std::string LastEventName = "";
    int LastCampaignId = 0;

    void TrackEvent(const std::string& eventName, int campaignId) override
    {
        TrackEventCalled = true;
        ++TrackEventCallCount;
        LastEventName = eventName;
        LastCampaignId = campaignId;
    }
};

void AssertTrue(bool condition, const std::string& testName)
{
    if (!condition)
    {
        throw std::runtime_error(testName + " failed");
    }
}

void AssertThrows(const std::function<void()>& action, const std::string& testName)
{
    bool thrown = false;

    try
    {
        action();
    }
    catch (...)
    {
        thrown = true;
    }

    if (!thrown)
    {
        throw std::runtime_error(testName + " failed");
    }
}

bool AreEqual(double left, double right, double epsilon = 1e-9)
{
    return std::abs(left - right) < epsilon;
}

int main()
{
    int passed = 0;
    int failed = 0;

    auto RunTest = [&](const std::string& testName, const std::function<void()>& testAction)
    {
        try
        {
            testAction();
            ++passed;
            std::cout << "pass: " << testName << '\n';
        }
        catch (const std::exception& exception)
        {
            ++failed;
            std::cout << "fail: " << testName << " -> " << exception.what() << '\n';
        }
    };

    RunTest("createcampaign valid calls add and track",
        [&]()
        {
            MockCampaignRepository repository;
            MockAnalyticsService analytics;
            CampaignService service(&repository, &analytics);

            service.CreateCampaign(1, "spring campaign", 1000.0);

            AssertTrue(repository.AddCalled, "createcampaign valid calls add and track");
            AssertTrue(repository.AddCallCount == 1, "createcampaign valid calls add and track");
            AssertTrue(repository.LastAddedCampaign.Id == 1, "createcampaign valid calls add and track");
            AssertTrue(analytics.TrackEventCalled, "createcampaign valid calls add and track");
            AssertTrue(analytics.LastEventName == "campaign_created", "createcampaign valid calls add and track");
        });

    RunTest("createcampaign invalid id throws",
        [&]()
        {
            MockCampaignRepository repository;
            MockAnalyticsService analytics;
            CampaignService service(&repository, &analytics);

            AssertThrows([&]() { service.CreateCampaign(0, "name", 100.0); }, "createcampaign invalid id throws");
            AssertTrue(repository.AddCallCount == 0, "createcampaign invalid id throws");
        });

    RunTest("createcampaign empty name throws",
        [&]()
        {
            MockCampaignRepository repository;
            MockAnalyticsService analytics;
            CampaignService service(&repository, &analytics);

            AssertThrows([&]() { service.CreateCampaign(1, "", 100.0); }, "createcampaign empty name throws");
            AssertTrue(analytics.TrackEventCallCount == 0, "createcampaign empty name throws");
        });

    RunTest("activatecampaign existing campaign returns true",
        [&]()
        {
            MockCampaignRepository repository;
            MockAnalyticsService analytics;
            CampaignService service(&repository, &analytics);
            Campaign campaign{7, "existing", 400.0, false};
            repository.GetByIdResult = &campaign;

            bool activated = service.ActivateCampaign(7);

            AssertTrue(activated, "activatecampaign existing campaign returns true");
            AssertTrue(repository.UpdateStatusCalled, "activatecampaign existing campaign returns true");
            AssertTrue(repository.LastUpdateStatusId == 7, "activatecampaign existing campaign returns true");
            AssertTrue(repository.LastUpdateStatusValue, "activatecampaign existing campaign returns true");
            AssertTrue(analytics.LastEventName == "campaign_activated", "activatecampaign existing campaign returns true");
        });

    RunTest("activatecampaign missing campaign returns false",
        [&]()
        {
            MockCampaignRepository repository;
            MockAnalyticsService analytics;
            CampaignService service(&repository, &analytics);
            repository.GetByIdResult = nullptr;

            bool activated = service.ActivateCampaign(99);

            AssertTrue(!activated, "activatecampaign missing campaign returns false");
            AssertTrue(repository.UpdateStatusCallCount == 0, "activatecampaign missing campaign returns false");
            AssertTrue(analytics.TrackEventCallCount == 0, "activatecampaign missing campaign returns false");
        });

    RunTest("calculateroi valid returns value and tracks",
        [&]()
        {
            MockCampaignRepository repository;
            MockAnalyticsService analytics;
            CampaignService service(&repository, &analytics);
            repository.GetBudgetResult = 1000.0;

            double roi = service.CalculateRoi(3, 1500.0);

            AssertTrue(AreEqual(roi, 0.5), "calculateroi valid returns value and tracks");
            AssertTrue(repository.GetBudgetCalled, "calculateroi valid returns value and tracks");
            AssertTrue(analytics.LastEventName == "roi_calculated", "calculateroi valid returns value and tracks");
        });

    RunTest("calculateroi negative revenue throws",
        [&]()
        {
            MockCampaignRepository repository;
            MockAnalyticsService analytics;
            CampaignService service(&repository, &analytics);

            AssertThrows([&]() { service.CalculateRoi(1, -1.0); }, "calculateroi negative revenue throws");
            AssertTrue(repository.GetBudgetCallCount == 0, "calculateroi negative revenue throws");
        });

    RunTest("calculateroi zero budget throws",
        [&]()
        {
            MockCampaignRepository repository;
            MockAnalyticsService analytics;
            CampaignService service(&repository, &analytics);
            repository.GetBudgetResult = 0.0;

            AssertThrows([&]() { service.CalculateRoi(1, 100.0); }, "calculateroi zero budget throws");
            AssertTrue(analytics.TrackEventCallCount == 0, "calculateroi zero budget throws");
        });

    RunTest("activatecampaign verify getbyid called once",
        [&]()
        {
            MockCampaignRepository repository;
            MockAnalyticsService analytics;
            CampaignService service(&repository, &analytics);
            repository.GetByIdResult = nullptr;

            service.ActivateCampaign(123);

            AssertTrue(repository.GetByIdCallCount == 1, "activatecampaign verify getbyid called once");
            AssertTrue(repository.LastGetByIdId == 123, "activatecampaign verify getbyid called once");
        });

    std::cout << "summary: passed=" << passed << " failed=" << failed << '\n';

    if (failed == 0)
    {
        std::cout << "all tests passed" << '\n';
        return 0;
    }

    std::cout << "some tests failed" << '\n';
    return 1;
}